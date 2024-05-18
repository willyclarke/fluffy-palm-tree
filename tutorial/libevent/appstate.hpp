/******************************************************************************
 * * Filename : appstate.hpp
 * * Date     : 2024 May 17
 * * Author   : Willy Clarke (willy@clarke.no)
 * * Version  : Use git you GIT
 * * Copyright: W. Clarke
 * * License  : MIT
 * * Descripti: Application state struct.
 * ******************************************************************************/
#ifndef TUTORIAL_LIBEVENT_APPSTATE_HPP_A74702DE_2DEE_4B79_AC6E_BDB328875191
#define TUTORIAL_LIBEVENT_APPSTATE_HPP_A74702DE_2DEE_4B79_AC6E_BDB328875191

#include <atomic>
#include <csignal>
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

#include <event2/event.h>

/**
 * Forward declarations.
 */
struct fsm_statemachine;

namespace app {
/**
 * Application state - for passing around state to the various threads.
 */
struct app_state {
  app_state() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
  ~app_state() { std::cout << __PRETTY_FUNCTION__ << std::endl; }

  bool TraceLevelSet{};

  event_base* Base{nullptr};
  event*      EventTimeout{nullptr};
  event*      EventReceiveLldpFrame{nullptr};
  event*      EventReceiveDcpFrame{nullptr};
  event*      EventReceiveIpFrame{nullptr};
  event*      pSignalInterrupt{nullptr};

  evutil_socket_t   SocketLLDPWrite{-1};
  evutil_socket_t   SocketLLDPRead{-1};
  evutil_socket_t   SocketDCP{-1};
  evutil_socket_t   SocketIP{-1};
  std::atomic<bool> Quit{};
  std::atomic<bool> SocketsAllocated{};
  std::atomic<int>  NumFsmInitialized{};
  std::atomic<int>  NumFsmExpected{};

  timeval LoopPeriod = {5, 0}; // X-second interval

  enum class return_code : int {
    OK        = 0,
    ALLOC_ERR = 1,
    ERR       = 2,
  };

  std::atomic<return_code> ReturnCode{};

  std::atomic<long> DCPFrameCount{};
  std::atomic<long> IPFrameCount{};
  std::atomic<long> LLDPFrameCount{};

  std::mutex                                     MutexvDeviceStateFsm{};
  std::vector<std::shared_ptr<fsm_statemachine>> vDeviceStateFsm{};
};
}; // end of namespace app

#endif
