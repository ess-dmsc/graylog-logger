/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Simple threaded executor to simplify the use of threading in this lib
/// library.
///
//===----------------------------------------------------------------------===//

#pragma once

#include <functional>
#include <future>
#include <memory>
#include <moodycamel/concurrentqueue.h>
#include <thread>

namespace Log {
class ThreadedExecutor {
private:
public:
  using WorkMessage = std::function<void()>;
  ThreadedExecutor() : WorkerThread(ThreadFunction) {}
  ~ThreadedExecutor() {
    SendWork([=]() { RunThread = false; });
    WorkerThread.join();
  }
  void SendWork(WorkMessage Message) { MessageQueue.enqueue(Message); }
  size_t size_approx() { return MessageQueue.size_approx(); }

private:
  bool RunThread{true};
  std::function<void()> ThreadFunction{[=]() {
    while (RunThread) {
      WorkMessage CurrentMessage;
      if (MessageQueue.try_dequeue(CurrentMessage)) {
        CurrentMessage();
      } else {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(5ms);
      }
    }
  }};
  moodycamel::ConcurrentQueue<WorkMessage> MessageQueue;
  std::thread WorkerThread;
};

} // namespace Log
