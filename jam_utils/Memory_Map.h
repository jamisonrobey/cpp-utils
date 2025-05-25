#pragma once
#include <sys/mman.h>
#include <system_error>

namespace jam_utils {
  class Memory_Map {
  public:
    explicit Memory_Map(const size_t length, const int prot, const int flags,
                        const int fd, const off_t offset) {
      addr_ = static_cast<std::byte *>(
        mmap(nullptr, length, prot, flags, fd, offset));
      if (addr_ == MAP_FAILED) {
        throw std::system_error(errno, std::system_category(), "mmap failed");
      }
      len_ = length;
    }

    Memory_Map() = default;


    Memory_Map(const Memory_Map &) = delete;
    Memory_Map &operator=(const Memory_Map &) = delete;

    Memory_Map(Memory_Map &&other) noexcept
      : addr_(other.addr_), len_(other.len_) {
      other.addr_ = static_cast<std::byte *>(MAP_FAILED);
      other.len_ = 0;
    }

    Memory_Map &operator=(Memory_Map &&other) noexcept {
      if (this != &other) {
        if (addr_ != MAP_FAILED) {
          munmap(addr_, len_);
        }

        addr_ = other.addr_;
        len_ = other.len_;

        other.addr_ = static_cast<std::byte *>(MAP_FAILED);
        other.len_ = 0;
      }
      return *this;
    }

    ~Memory_Map() {
      if (addr_ != MAP_FAILED) {
        munmap(addr_, len_);
      }
    }

    const std::byte *get_addr() const { return addr_; }
    size_t get_len() const { return len_; }

  private:
    std::byte *addr_ = static_cast<std::byte *>(MAP_FAILED);
    size_t len_ = 0;
  };
}