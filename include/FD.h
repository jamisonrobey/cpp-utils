#pragma once

#include <fcntl.h>
#include <format>
#include <stdexcept>
#include <system_error>
#include <unistd.h>

class FD {

  FD(const int fd) {
    if (fd < 0) {
      throw std::runtime_error("Invalid FD");
    }
  }

  FD(const std::string &file_path, const int oflag = O_RDONLY) {
    m_fd = open(file_path.c_str(), oflag);
    if (!valid()) {
      throw std::system_error(errno, std::system_category(),
                              std::format("Could not open file {}", file_path));
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

private:
  int m_fd = -1;
  bool valid() const noexcept { return m_fd >= 0; }
};