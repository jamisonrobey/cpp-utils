#pragma once

#include <fcntl.h>
#include <format>
#include <stdexcept>
#include <unistd.h>

namespace jam_utils {
  class FD {
  public:
    explicit FD(const int fd) : m_fd(fd) {
      if (!valid()) {
        throw std::runtime_error("Got a bad FD");
      }
    }

    explicit FD(const std::string &file_path, const int oflags = O_RDONLY)
        : m_fd(open(file_path.c_str(), oflags)) {
      if (!valid()) {
        throw std::runtime_error(
            std::format("Failed to get fd for file at path: {}", file_path));
      }
    }

    FD() = delete;

    FD(const FD &) = delete;
    FD &operator=(const FD &) = delete;

    FD(FD &&other) noexcept : m_fd(other.m_fd) { other.m_fd = -1; }
    FD &operator=(FD &&other) noexcept {
      if (this != &other) {
        if (valid()) {
          close(m_fd);
        }
        m_fd = other.m_fd;
        other.m_fd = -1;
      }
      return *this;
    }

    ~FD() noexcept {
      if (valid()) {
        close(m_fd);
      }
    }

    int fd() const noexcept { return m_fd; }
    bool valid() const noexcept { return m_fd >= 0; }

  private:
    int m_fd = -1;
  };
};
