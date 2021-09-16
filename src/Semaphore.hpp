#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>

class Semaphore {
public:
  void notify() {
    std::lock_guard<decltype(Mutex)> lock(Mutex);
    ++Count;
    Condition.notify_one();
  }

  void wait() {
    std::unique_lock<decltype(Mutex)> lock(Mutex);
    while (!Count) // Handle spurious wake-ups.
      Condition.wait(lock);
    --Count;
  }

private:
  std::mutex Mutex;
  std::condition_variable Condition;
  std::atomic<int> Count{0}; // Initialized as locked.
};