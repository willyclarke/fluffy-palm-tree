#include <event2/event.h>

#include <atomic>
#include <csignal>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "appstate.hpp"
#include "statemachine.hpp"

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
        StartupComplete       = !Empty && pAppState->SocketsAllocated;
        StartupComplete &= pAppState->NumFsmExpected == pAppState->NumFsmInitialized;
      }

      if (StartupComplete)
        break;

      --WaitCnt;

      using namespace std::chrono_literals;
      std::this_thread::sleep_for(1ms);
    }

    if (pAppState->pSignalInterrupt)
      event_del(pAppState->pSignalInterrupt);
    if (pAppState->EventReceiveLldpFrame)
      event_del(pAppState->EventReceiveLldpFrame);
    if (pAppState->EventReceiveDcpFrame)
      event_del(pAppState->EventReceiveDcpFrame);
    if (pAppState->EventReceiveIpFrame)
      event_del(pAppState->EventReceiveIpFrame);
    if (pAppState->EventTimeout)
      event_del(pAppState->EventTimeout);

    {
      std::lock_guard Lock(pAppState->MutexvDeviceStateFsm);
      for (auto pDeviceStateFsm : pAppState->vDeviceStateFsm)
      {
         pDeviceStateFsm->Stop();
      }
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

  /* Start event handling */
  event_base_dispatch(pEventBase);

  if (pSignalInterrupt)
    event_free(pSignalInterrupt);
  if (pEventBase)
    event_base_free(pEventBase);

  return static_cast<int>(app_state::return_code::OK);
}

/**
 */
auto Run(app::app_state* pAppState) -> void {
  std::cout << __PRETTY_FUNCTION__ << " -> Entry." << std::endl;

  std::vector<std::thread> vThreads{};
  {

    auto pDeviceStateFsm = std::make_shared<fsm_statemachine>(pAppState);

    if (!pDeviceStateFsm)
      return;

    std::lock_guard Lock(pAppState->MutexvDeviceStateFsm);
    pAppState->vDeviceStateFsm.push_back(pDeviceStateFsm);
    vThreads.push_back(std::thread{&fsm_statemachine::Run, &(*pDeviceStateFsm)});
  }

  {
    // std::lock_guard Lock(pAppState->MutexvDeviceStateFsm);
    for (auto& T : vThreads) {
      if (T.joinable())
        T.join();
    }
  }

  /* Try to clean up all globals as well */
  libevent_global_shutdown();

  std::cout << __PRETTY_FUNCTION__ << " -> Exit." << std::endl;
}
}; // end namespace app

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

  return 0;
}
