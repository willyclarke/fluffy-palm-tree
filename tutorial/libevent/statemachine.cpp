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
fsm_statemachine::fsm_statemachine(app::app_state* pAS, char const* pID) : pAppState(pAS) {

  if (pID)
    ID = std::string(pID);

  std::cout << __PRETTY_FUNCTION__ << ". Object no. " << this << ". ID: " << ID << std::endl;

  pEventBase = event_base_new();
  if (!pEventBase)
    return;

  pEventGotoStateInit = event_new(pEventBase, -1, 0, StateInit, (void*)this);
  pEventGotoStateA    = event_new(pEventBase, -1, 0, StateA, (void*)this);
  pEventGotoStateB    = event_new(pEventBase, -1, 0, StateB, (void*)this);
  pEventGotoStateC    = event_new(pEventBase, -1, 0, StateC, (void*)this);
  pEventKeepAlive     = event_new(pEventBase, -1, 0, StateKeepAlive, (void*)this);

  event_add(pEventGotoStateInit, NULL);
  event_add(pEventGotoStateA, NULL);
  event_add(pEventGotoStateB, NULL);
  event_add(pEventGotoStateC, NULL);
  event_add(pEventKeepAlive, NULL);

  /* Move to StateInit immediately. */
  event_active(pEventKeepAlive, 0, 0);
  event_active(pEventGotoStateInit, 0, 0);

  std::cout << __PRETTY_FUNCTION__ << ". EXIT CTOR" << std::endl;
}

/**
 *
 */
fsm_statemachine::~fsm_statemachine() {
  std::cout << this << " :: " << __PRETTY_FUNCTION__ << ". ID: " << ID << std::endl;

  if (!pEventBase) {
    std::cout << __PRETTY_FUNCTION__ << ". EXIT DTOR Abnormally." << std::endl;
    return;
  }

  if (pEventGotoStateInit) {
    event_del(pEventGotoStateInit);
    event_free(pEventGotoStateInit);
  }

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

  if (pEventKeepAlive) {
    event_del(pEventKeepAlive);
    event_free(pEventKeepAlive);
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
  std::cout << this << " :: " << __PRETTY_FUNCTION__ << ". ID: " << ID << std::endl;

  if (!pEventBase)
    return;

  /* Start event handling */
  event_base_dispatch(pEventBase);

  std::cout << this << " :: " << __PRETTY_FUNCTION__ << ". ID: " << ID << ". EXIT." << std::endl;
}

/**
 *
 */
void fsm_statemachine::Stop() {

  if (pEventBase) {
    std::cout << this << " :: " << __PRETTY_FUNCTION__ << ". ID: " << ID << std::endl;

    event_del(pEventGotoStateInit);
    event_del(pEventGotoStateA);
    event_del(pEventGotoStateB);
    event_del(pEventGotoStateC);
    event_del(pEventKeepAlive);

    timeval const ExitWaitTime = {0, 500}; // X-second, μ-second interval
    event_base_loopexit(pEventBase, &ExitWaitTime);
    event_base_loopbreak(pEventBase);
  }

  std::cout << this << " :: " << __PRETTY_FUNCTION__ << ". ID: " << ID << ". EXIT." << std::endl;
}

/**
 */
void fsm_statemachine::Trig(char C) {
  switch (C) {
  case 'a':
  case 'A': {
    event_active(pEventGotoStateA, 0, 0);
  } break;
  case 'b':
  case 'B': {
    event_active(pEventGotoStateB, 0, 0);
  } break;
  case 'c':
  case 'C': {
    event_active(pEventGotoStateC, 0, 0);
  } break;
  case 'i':
  case 'I': {
    event_active(pEventGotoStateInit, 0, 0);
  } break;
  default:
    break;
  }
}

/**
 *
 */
void fsm_statemachine::StateKeepAlive(evutil_socket_t Sockfd, short Event, void* pArg) {
  auto pFsm = (fsm_statemachine*)pArg;

  auto pAppState = pFsm->pAppState;
  if (!pAppState)
    return;

  if (pFsm->pAppState->Quit) {
    std::cout << pArg << " :: " << __PRETTY_FUNCTION__ << ". ID: " << pFsm->ID << " -> Quit detected. " << std::endl;
    return;
  }

  /* Schedule update to run after ... */
  event_add(pFsm->pEventKeepAlive, &pAppState->LoopPeriod);
}

/**
 *
 */
void fsm_statemachine::StateInit(evutil_socket_t Sockfd, short Event, void* pArg) {
  auto pFsm = (fsm_statemachine*)pArg;
  std::cout << pArg << " :: " << __PRETTY_FUNCTION__ << ". ID: " << pFsm->ID << std::endl;
}

/**
 *
 */
void fsm_statemachine::StateA(evutil_socket_t Sockfd, short Event, void* pArg) {
  auto pFsm = (fsm_statemachine*)pArg;
  std::cout << pArg << " :: " << __PRETTY_FUNCTION__ << ". ID: " << pFsm->ID << std::endl;
}

/**
 *
 */
void fsm_statemachine::StateB(evutil_socket_t Sockfd, short Event, void* pArg) {
  auto pFsm = (fsm_statemachine*)pArg;
  std::cout << pArg << " :: " << __PRETTY_FUNCTION__ << ". ID: " << pFsm->ID << std::endl;
}

/**
 *
 */
void fsm_statemachine::StateC(evutil_socket_t Sockfd, short Event, void* pArg) {
  auto pFsm = (fsm_statemachine*)pArg;
  std::cout << pArg << " :: " << __PRETTY_FUNCTION__ << ". ID: " << pFsm->ID << std::endl;

  if (pFsm->pAppState->Quit)
    std::cout << pArg << " :: " << __PRETTY_FUNCTION__ << ". ID: " << pFsm->ID << " -> Quit detected. " << std::endl;
}
