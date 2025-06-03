#pragma once

#include <fcntl.h>
#include <format>
#include <stdexcept>
#include <unistd.h>

namespace jam_utils {
  class FD {
  public:
    explicit FD(const int fd) : fd_(fd) {
      if (!valid()) {
        throw std::runtime_error("Got a bad FD");
      }
    }

    explicit FD(std::string_view file_path, const int oflags = O_RDONLY)
      : fd_(open(file_path.data(), oflags)) {
      if (!valid()) {
        throw std::runtime_error(
            std::format("Failed to get fd for file at path: {}", file_path));
      }
    }

    FD() : fd_(-1) {}

    FD(const FD &) = delete;
    FD &operator=(const FD &) = delete;

    FD(FD &&other) noexcept : fd_(other.fd_) { other.fd_ = -1; }

    FD &operator=(FD &&other) noexcept {
      if (this != &other) {
        if (valid()) {
          close(fd_);
        }
        fd_ = other.fd_;
        other.fd_ = -1;
      }
      return *this;
    }

    ~FD() noexcept {
      if (valid()) {
        close(fd_);
      }
    }

    int fd() const noexcept { return fd_; }
    bool valid() const noexcept { return fd_ >= 0; }

  private:
    int fd_;
  };
};