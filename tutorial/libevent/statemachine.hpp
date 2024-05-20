/******************************************************************************
 * * Filename : statemachine.hpp
 * * Date     : 2024 May 17
 * * Author   : Willy Clarke (willy@clarke.no)
 * * Version  : Use git you GIT
 * * Copyright: W. Clarke
 * * License  : MIT
 * * Descripti: Simple state machine for testing of libevent.
 * ******************************************************************************/
#ifndef TUTORIAL_LIBEVENT_HPP_03CF020A_E536_4218_8793_1E0F47290EE3
#define TUTORIAL_LIBEVENT_HPP_03CF020A_E536_4218_8793_1E0F47290EE3

#include <string>

#include <event2/event.h>

#include "appstate.hpp"

struct fsm_statemachine {
  fsm_statemachine() = delete;
  fsm_statemachine(app::app_state* pAS, char const* pID);
  ~fsm_statemachine();

  std::string ID{};

  app::app_state* pAppState{nullptr};
  event_base*     pEventBase{nullptr};
  event*          pEventDataExchange{nullptr};
  event*          pEventGotoStateInit{nullptr};
  event*          pEventGotoStateA{nullptr};
  event*          pEventGotoStateB{nullptr};
  event*          pEventGotoStateC{nullptr};
  event*          pEventKeepAlive{nullptr};

  void Run();
  void Stop();
  void Trig(char C);

  static void StateKeepAlive(evutil_socket_t Sockfd, short Event, void* pArg);
  static void StateInit(evutil_socket_t Sockfd, short Event, void* pArg);
  static void StateA(evutil_socket_t Sockfd, short Event, void* pArg);
  static void StateB(evutil_socket_t Sockfd, short Event, void* pArg);
  static void StateC(evutil_socket_t Sockfd, short Event, void* pArg);
};
#endif
