/******************************************************************************
 * * Filename : statemachine.cpp
 * * Date     : 2024 May 17
 * * Author   : Willy Clarke (willy@clarke.no)
 * * Version  : Use git you GIT
 * * Copyright: W. Clarke
 * * License  : MIT
 * * Descripti: Simple statemachine for testing of libevent.
 * ******************************************************************************/
#include <iostream>

#include "statemachine.hpp"

/**
 *
 */
fsm_statemachine::fsm_statemachine(app::app_state* pAS) : pAppState(pAS) {

  std::cout << __PRETTY_FUNCTION__ << std::endl;
  pEventBase = event_base_new();
  if (!pEventBase)
    return;

  pEventGotoStateInit = event_new(pEventBase, -1, 0, StateInit, (void*)this);
  pEventGotoStateA    = event_new(pEventBase, -1, 0, StateA, (void*)this);
  pEventGotoStateB    = event_new(pEventBase, -1, 0, StateB, (void*)this);
  pEventGotoStateC    = event_new(pEventBase, -1, 0, StateC, (void*)this);

  /* Move to StateInit immediately. */
  event_add(pEventGotoStateInit, NULL);
  event_active(pEventGotoStateInit, 0, 0);

  std::cout << __PRETTY_FUNCTION__ << ". EXIT CTOR" << std::endl;
}

/**
 *
 */
fsm_statemachine::~fsm_statemachine() {

  if (!pEventBase) {
    std::cout << __PRETTY_FUNCTION__ << ". EXIT DTOR Abnormally." << std::endl;
    return;
  }

  if (pEventGotoStateInit) {
    event_del(pEventGotoStateInit);
    event_free(pEventGotoStateInit);
  }
  std::cout << __PRETTY_FUNCTION__ << ". Line:" << __LINE__ << std::endl;
  if (pEventGotoStateA) {
    event_del(pEventGotoStateA);
    event_free(pEventGotoStateA);
  }
  if (pEventGotoStateB) {
    event_del(pEventGotoStateB);
    event_free(pEventGotoStateB);
  }
  if (pEventGotoStateC) {
    event_del(pEventGotoStateC);
    event_free(pEventGotoStateC);
  }

  Stop();

  /* Must come after freeing of events */
  if (pEventBase)
    event_base_free(pEventBase);

  std::cout << __PRETTY_FUNCTION__ << ". EXIT DTOR Normally." << std::endl;
}

/**
 *
 */
void fsm_statemachine::Run() {

  std::cout << __PRETTY_FUNCTION__ << ". ENTER." << std::endl;

  if (!pEventBase)
    return;

  /* Start event handling */
  event_base_dispatch(pEventBase);

  std::cout << __PRETTY_FUNCTION__ << ". EXIT." << std::endl;
}

/**
 *
 */
void fsm_statemachine::Stop() {

  std::cout << __PRETTY_FUNCTION__ << ". Line:" << __LINE__ << std::endl;

  if (pEventBase) {
    event_base_loopbreak(pEventBase);
    timeval const ExitWaitTime = {1, 1000 * 500}; // X-second, μ-second interval
    event_base_loopexit(pEventBase, &ExitWaitTime);
  }

  std::cout << __PRETTY_FUNCTION__ << ". EXIT." << std::endl;
}

/**
 *
 */
void fsm_statemachine::StateInit(evutil_socket_t Sockfd, short Event, void* pArg) {

  auto pDeviceStateFsm = (fsm_statemachine*)pArg;

  std::cout << __PRETTY_FUNCTION__ << std::endl;

  /* Add myself as a pending event so that I can be called. */
  event_add(pDeviceStateFsm->pEventGotoStateInit, NULL);

  // if (pDeviceStateFsm->pAppState->Quit)
  //   return;

  /* Go to the next state. */
  event_active(pDeviceStateFsm->pEventGotoStateA, 0, 0);
}

/**
 *
 */
void fsm_statemachine::StateA(evutil_socket_t Sockfd, short Event, void* pArg) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;

  auto pDeviceStateFsm = (fsm_statemachine*)pArg;

  /* Add myself as a pending event so that I can be called. */
  event_add(pDeviceStateFsm->pEventGotoStateA, NULL);

  // if (pDeviceStateFsm->pAppState->Quit)
  //   return;

  /* Go to the next state. */
  event_active(pDeviceStateFsm->pEventGotoStateB, 0, 0);
}

/**
 *
 */
void fsm_statemachine::StateB(evutil_socket_t Sockfd, short Event, void* pArg) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;

  auto pDeviceStateFsm = (fsm_statemachine*)pArg;

  /* Add myself as a pending event so that I can be called. */
  event_add(pDeviceStateFsm->pEventGotoStateB, NULL);

  // if (pDeviceStateFsm->pAppState->Quit)
  //   return;

  /* Go to the next state. */
  event_active(pDeviceStateFsm->pEventGotoStateC, 0, 0);
}

/**
 *
 */
void fsm_statemachine::StateC(evutil_socket_t Sockfd, short Event, void* pArg) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  auto pDeviceStateFsm = (fsm_statemachine*)pArg;

  if (pDeviceStateFsm->pAppState->Quit)
    std::cout << __PRETTY_FUNCTION__ << " -> Quit detected. " << std::endl;

  //   return;

  /* Add myself as a pending event so that I can be called. */
  event_add(pDeviceStateFsm->pEventGotoStateC, NULL);

  /* Go to the next state. */
  event_active(pDeviceStateFsm->pEventGotoStateInit, 0, 0);
}
