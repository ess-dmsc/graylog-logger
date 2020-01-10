/* Copyright (C) 2020 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief A simple dummy log handler.
///
//===----------------------------------------------------------------------===//


#include "DummyLogHandler.h"

// This code is here instead of in the header file to prevent the compiler
// from optimising the code away.
void DummyLogHandler::addMessage(const Log::LogMessage &Message) {
  Executor.SendWork([=]() {});
}