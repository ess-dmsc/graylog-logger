/* Copyright (C) 2019 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Graylog connection status.
///
//===----------------------------------------------------------------------===//
#pragma once

namespace Log {
enum class Status {
  ADDR_LOOKUP,
  ADDR_RETRY_WAIT,
  CONNECT,
  SEND_LOOP,
};
} // namespace Log
