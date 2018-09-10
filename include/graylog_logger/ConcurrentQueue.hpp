/* Copyright (C) 2018 European Spallation Source, ERIC. See LICENSE file */
//===----------------------------------------------------------------------===//
///
/// \file
///
/// \brief Simple thread safe message queue.
///
//===----------------------------------------------------------------------===//


#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

template <typename Data> class ConcurrentQueue {
private:
  std::queue<Data> the_queue;
  mutable std::mutex the_mutex;
  std::condition_variable the_condition_variable;

public:
  void push(Data const &data) {
    {
      std::lock_guard<std::mutex> lock(the_mutex);
      the_queue.push(data);
    }
    the_condition_variable.notify_one();
  }

  unsigned long size() {
    unsigned long retVal;
    std::lock_guard<std::mutex> lock(the_mutex);
    retVal = the_queue.size();
    return retVal;
  }

  bool empty() const {
    std::lock_guard<std::mutex> lock(the_mutex);
    return the_queue.empty();
  }

  bool try_pop(Data &popped_value) {
    std::lock_guard<std::mutex> lock(the_mutex);
    if (the_queue.empty()) {
      return false;
    }

    popped_value = the_queue.front();
    the_queue.pop();
    return true;
  }

  bool try_pop() {
    std::lock_guard<std::mutex> lock(the_mutex);
    if (the_queue.empty()) {
      return false;
    }
    the_queue.pop();
    return true;
  }

  bool try_peek(Data &peek_value) {
    std::lock_guard<std::mutex> lock(the_mutex);
    if (the_queue.empty()) {
      return false;
    }

    peek_value = the_queue.front();
    return true;
  }

  void wait_and_pop(Data &popped_value) {
    std::unique_lock<std::mutex> lock(the_mutex);
    while (the_queue.empty()) {
      the_condition_variable.wait(lock);
    }

    popped_value = the_queue.front();
    the_queue.pop();
  }

  void wait_and_peek(Data &peek_value) {
    std::unique_lock<std::mutex> lock(the_mutex);
    while (the_queue.empty()) {
      the_condition_variable.wait(lock);
    }

    peek_value = the_queue.front();
  }

  bool time_out_pop(Data &popped_value, int mSec = 500) {
    std::unique_lock<std::mutex> lock(the_mutex);
    if (the_queue.empty()) {
      the_condition_variable.wait_for(lock, std::chrono::milliseconds(mSec));
    }

    if (the_queue.empty()) {
      return false;
    }

    popped_value = the_queue.front();
    the_queue.pop();
    return true;
  }

  bool time_out_peek(Data &peek_value, int mSec = 500) {
    std::unique_lock<std::mutex> lock(the_mutex);
    if (the_queue.empty()) {
      the_condition_variable.wait_for(lock, std::chrono::milliseconds(mSec));
    }

    if (the_queue.empty()) {
      return false;
    }

    peek_value = the_queue.front();
    return true;
  }
};
