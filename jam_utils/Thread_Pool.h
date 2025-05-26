#pragma once

#include "Safe_Queue.h"

#include <functional>
#include <thread>
#include <vector>

namespace jam_utils {
  class Thread_Pool {
    std::vector<std::jthread> threads_;
    jam_utils::Safe_Queue<std::function<void()>> tasks_;
    std::stop_source stop_src_;

  public:
    explicit Thread_Pool(
        const size_t num_threads = std::thread::hardware_concurrency()) {
      for (auto i = 0; i < num_threads; ++i) {
        threads_.emplace_back([this](std::stop_token st) {
          while (!st.stop_requested()) {
            if (const auto task = tasks_.wait_and_pop(st)) {
              (*task)();
            } else {
              break;
            }
          }
        });
      }
    }

    ~Thread_Pool() {
      for (auto &thread : threads_) {
        thread.request_stop();
      }
    }

    template <typename F> void enqueue(F &&f) {
      tasks_.emplace(std::forward<F>(f));
    }
  };
}