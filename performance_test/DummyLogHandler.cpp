#include "DummyLogHandler.h"

void DummyLogHandler::addMessage(const Log::LogMessage &Message) {
  Executor.SendWork([=]() {
  });
}