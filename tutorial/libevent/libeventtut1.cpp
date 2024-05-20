#include <event2/event.h>

#include <atomic>
#include <csignal>
#include <cstdio>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <termios.h>
#include <unistd.h> // For STDIN_FILENO

#include "appstate.hpp"
#include "statemachine.hpp"

#include <fcntl.h> //!< to make reading from stdin non-blocking.

namespace app {

/**
 * The SignalCallback will do an event_del to stop the event handling so that
 * the application can continue its exit process.
 */
static void SignalCallback(evutil_socket_t Socket, short Event, void* pArg) {
  app::app_state* pAppState = (app::app_state*)pArg;

  if (!pAppState)
    return;
  if (!pAppState->pSignalInterrupt)
    return;

  auto Signal = event_get_signal(pAppState->pSignalInterrupt);

  std::cout << __PRETTY_FUNCTION__ << " -> Got signal " << Signal << std::endl;

  if (SIGINT == Signal || SIGTERM == Signal) {
    pAppState->Quit         = true;
    size_t constexpr MAXCNT = 60;
    size_t WaitCnt{MAXCNT};

    bool StartupComplete{};
    /**
     * Wait for the FSM object to occur so that it can be shut down properly.
     */
    while (!StartupComplete && WaitCnt) {
      {
        std::lock_guard Lock(pAppState->MutexvDeviceStateFsm);
        bool const      Empty = pAppState->vDeviceStateFsm.empty();
        StartupComplete       = !Empty;
      }

      if (StartupComplete)
        break;

      --WaitCnt;

      using namespace std::chrono_literals;
      std::this_thread::sleep_for(1ms);
    }

    if (pAppState->pSignalInterrupt)
      event_del(pAppState->pSignalInterrupt);

    {
      std::lock_guard Lock(pAppState->MutexvDeviceStateFsm);
      for (auto pDeviceStateFsm : pAppState->vDeviceStateFsm) {
        pDeviceStateFsm->Stop();
      }
    }

    {
      event_base_loopbreak(pAppState->pEventBase);
      timeval const ExitWaitTime = {1, 1000 * 500}; // X-second, μ-second interval
      event_base_loopexit(pAppState->pEventBase, &ExitWaitTime);
    }
  }
  std::cout << __PRETTY_FUNCTION__ << " -> EXIT. Got signal " << Signal << std::endl;
}

/**
 * Make a file descriptor non-blocking.
 */
void SetNonblocking(evutil_socket_t Socketfd) {
  int flags = fcntl(Socketfd, F_GETFL, 0);
  if (flags != -1) {
    int const Result = fcntl(Socketfd, F_SETFL, flags | O_NONBLOCK);
    std::cout << __PRETTY_FUNCTION__ << " -> Second call returned " << Result << std::endl;
  }
}

/**
 * Make terminal non-canonical.
 * Create an instance of this object to make the terminal non-blocking.
 * When the object goes out of scope the terminal will be reset back to
 * the settings it had upon construction.
 */
struct set_noncanonical_mode {

  termios original_termios{};

  set_noncanonical_mode() {
    termios t{};
    tcgetattr(STDIN_FILENO, &t);
    original_termios = t;          /* Save the original_termios */
    t.c_lflag &= ~(ICANON | ECHO); /* Disable canonical mode and echo. */
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
    // std::cout << __PRETTY_FUNCTION__ << " -> set_noncanonical_mode on terminal." << std::endl;
  }
  ~set_noncanonical_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
    // std::cout << __PRETTY_FUNCTION__ << " -> reset of set_noncanonical_mode." << std::endl;
  }
};

/**
 */
void KeyPressed(evutil_socket_t Sockfd, short Event, void* pArg) {
  app::app_state* pAppState = (app::app_state*)pArg;
  // static long     Cnt{};
  // ++Cnt;
  // std::cout << pAppState << " :: " << __PRETTY_FUNCTION__ << ". Cnt: " << Cnt << std::endl;

  // Check if the event is a read event
  if (Event & EV_READ) {
    // Read data from stdin
    char    Buffer[64]{};
    ssize_t n = read(Sockfd, Buffer, sizeof(Buffer));
    if (n > 0) {
      // Process the read data
      // std::cout << "Received data from stdin: " << std::string(Buffer, n) << std::endl;

      for (auto const& Fsm : pAppState->vDeviceStateFsm) {
        Fsm->Trig(Buffer[0]);
      }

    } else if (n == 0) {
      // EOF (end of file) received, stdin has been closed
      std::cout << "EOF received on stdin" << std::endl;
    } else {
      // Error reading from stdin
      std::cerr << "Error reading from stdin" << std::endl;
    }
  }
}

/**
 */
auto WaitForSignal(app::app_state* pAppState) -> int {
  std::cout << "Hello World!" << std::endl;

  /* Initialize the event library */
  event_base* pEventBase = event_base_new();

  if (!pEventBase)
    return static_cast<int>(app_state::return_code::ALLOC_ERR);

  pAppState->pEventBase = pEventBase;

  /* Initialize one event */
  event* pSignalInterrupt = evsignal_new(pEventBase, SIGINT, SignalCallback, (void*)pAppState);

  if (!pSignalInterrupt) {
    if (pEventBase)
      event_base_free(pEventBase);
    return static_cast<int>(app_state::return_code::ALLOC_ERR);
  }

  /* Copy the pointer to app_state so that the interrupt routine can delete it. */
  pAppState->pSignalInterrupt = pSignalInterrupt;

  event_add(pAppState->pSignalInterrupt, NULL);

  /* Capture incoming characters. */
  evutil_socket_t StdIn = 0;

  // pAppState->pKeyPress = event_new(pEventBase, StdIn, EV_READ | EV_PERSIST, KeyPressed, (void*)pAppState);
  pAppState->pKeyPress = event_new(pEventBase, StdIn, EV_READ | EV_PERSIST, KeyPressed, (void*)pAppState);

  if (pAppState->pKeyPress)
    event_add(pAppState->pKeyPress, NULL);

  /* Start event handling */
  event_base_dispatch(pEventBase);

  if (pSignalInterrupt)
    event_free(pSignalInterrupt);

  if (pAppState->pKeyPress)
    event_free(pAppState->pKeyPress);

  if (pEventBase)
    event_base_free(pEventBase);

  return static_cast<int>(app_state::return_code::OK);
}

/**
 */
auto Run(app::app_state* pAppState) -> void {
  std::cout << __PRETTY_FUNCTION__ << " -> Entry." << std::endl;

  std::vector<std::string> const vIDS{"ID1", "ID2"};
  std::vector<std::thread>       vThreads{};

  for (auto const& ID : vIDS) {

    auto pDeviceStateFsm = std::make_shared<fsm_statemachine>(pAppState, ID.c_str());

    if (!pDeviceStateFsm)
      return;

    std::lock_guard Lock(pAppState->MutexvDeviceStateFsm);
    pAppState->vDeviceStateFsm.push_back(pDeviceStateFsm);
    vThreads.push_back(std::thread{&fsm_statemachine::Run, &(*pDeviceStateFsm)});
  }

  {
    for (auto& T : vThreads) {
      if (T.joinable())
        T.join();
    }
  }

  std::cout << __PRETTY_FUNCTION__ << " -> Exit." << std::endl;
}
}; // end namespace app

/* Set up terminal to be non-blocking. */
app::set_noncanonical_mode gSetCanonicalMode{};

/**
 */
auto main() -> int {

  auto pAppState = std::make_shared<app::app_state>();
  if (!pAppState)
    return -1;

  std::thread TWaitForSignal(app::WaitForSignal, pAppState.get());
  std::thread TRun(app::Run, pAppState.get());

  if (TRun.joinable())
    TRun.join();

  if (TWaitForSignal.joinable())
    TWaitForSignal.join();

  pAppState.reset();

  std::cout << __PRETTY_FUNCTION__ << " -> Normal exit ..." << std::endl;

  /* Try to clean up all globals as well */
  libevent_global_shutdown();

  return 0;
}
