#pragma once

#include "Safe_Queue.h"

#include <functional>
#include <thread>
#include <vector>

namespace jam_utils {
  class Thread_Pool {
    std::vector<std::jthread> threads_;
    jam_utils::Safe_Queue<std::function<void()>> tasks_;

  public:
    explicit Thread_Pool(
        const size_t num_threads = std::thread::hardware_concurrency()) {
      for (size_t i = 0; i < num_threads; ++i) {
        threads_.emplace_back([this](const std::stop_token &st) {
          while (!st.stop_requested()) {
            if (const auto task = tasks_.wait_and_pop(st)) {
              (*task)();
            }
          }
        });
      }
    }

    ~Thread_Pool() {
      for (auto &th : threads_) {
        th.request_stop();
      }
    }

    template <typename F> void enqueue(F &&f) {
      tasks_.emplace(std::forward<F>(f));
    }
  };
}