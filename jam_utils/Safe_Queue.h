#pragma once

#include <concepts>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include <stop_token>

// I used C++ concurrency in action (2nd ed.) listing 4.5 as a reference and
// just adapted to use optional over bool and shared ptr this forces T to be
// noexcept moveable which is not the end of the world

namespace jam_utils {

  template <typename T>
    requires std::is_nothrow_move_constructible_v<T>
  class Safe_Queue {
    mutable std::mutex mut_;
    std::queue<T> queue_;
    std::condition_variable_any cond_;

  public:
    Safe_Queue() = default;

    Safe_Queue(const Safe_Queue &other) {
      std::scoped_lock lock(other.mut_);
      queue_ = other.queue_;
    }

    Safe_Queue(Safe_Queue &&other) noexcept {
      std::scoped_lock lock(other.mut_);
      queue_ = std::move(other.queue_);
    }

    Safe_Queue &operator=(const Safe_Queue &other) {
      if (this != &other) {
        if (this < &other) {
          std::scoped_lock lock(mut_, other.mut_);
          queue_ = other.queue_;
        } else {
          std::scoped_lock lock(other, mut_);
          queue_ = other.queue_;
        }
        return *this;
      }
    }

    Safe_Queue &operator=(Safe_Queue &&other) noexcept {
      if (this != &other) {
        if (this < &other) {
          std::scoped_lock lock(mut_, other.mut_);
          queue_ = std::move(other.queue_);
        } else {
          std::scoped_lock lock(other.mut_, mut_);
          queue_ = std::move(other.queue_);
        }
      }
      return *this;
    }

    void push(const T &item) {
      std::scoped_lock lock(mut_);
      queue_.push(item);
      cond_.notify_one();
    }

    void push(T &&item) {
      std::scoped_lock lock(mut_);
      queue_.push(std::move(item));
      cond_.notify_one();
    }

    template <typename... Args> void emplace(Args &&...args) {
      std::scoped_lock lock(mut_);
      queue_.emplace(std::forward<Args>(args)...);
      cond_.notify_one();
    }

    [[nodiscard]] std::optional<T> try_pop() {
      std::scoped_lock lock(mut_);
      if (queue_.empty()) {
        return std::nullopt;
      }
      T result = std::move(queue_.front());
      queue_.pop();
      return result;
    }

    T wait_and_pop() {
      std::unique_lock lock(mut_);
      cond_.wait(lock, [this] { return !queue_.empty(); });
      T result = std::move(queue_.front());
      queue_.pop();
      return result;
    }

    [[nodiscard]] std::optional<T> wait_and_pop(std::stop_token st) {
      std::unique_lock lock(mut_);
      if (cond_.wait(lock, st, [this] { return !queue_.empty(); })) {
        T result = std::move(queue_.front());
        queue_.pop();
        return result;
      }
      return std::nullopt;
    }

    [[nodiscard]] bool empty() const {
      std::scoped_lock lock(mut_);
      return queue_.empty();
    }

    [[nodiscard]] size_t size() const {
      std::scoped_lock lock(mut_);
      return queue_.size();
    }

    void clear() {
      std::scoped_lock lock(mut_);
      queue_ = {};
    }

    void swap(Safe_Queue &other) noexcept {
      if (this != &other) {
        std::scoped_lock lock(mut_, other.mut_);
        queue_.swap(other.queue_);
      }
    }
  };

  template <typename T>
    requires std::movable<T>
  void swap(Safe_Queue<T> &lhs, Safe_Queue<T> &rhs) noexcept {
    lhs.swap(rhs);
  }
}