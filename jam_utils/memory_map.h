#pragma once
#include <sys/mman.h>
#include <system_error>

namespace jam_utils {
  // RAII wrapper for memory mapped region represented / cast as std::byte*
  class Memory_Map {
  public:
    explicit Memory_Map(const size_t length, const int prot, const int flags,
                        const int fd, const off_t offset) {
      m_addr = static_cast<std::byte *>(
        mmap(nullptr, length, prot, flags, fd, offset));
      if (m_addr == MAP_FAILED) {
        throw std::system_error(errno, std::system_category(), "mmap failed");
      }
      m_len = length;
    }

    Memory_Map() = default;


    Memory_Map(const Memory_Map &) = delete;
    Memory_Map &operator=(const Memory_Map &) = delete;

    Memory_Map(Memory_Map &&other) noexcept
      : m_addr(other.m_addr), m_len(other.m_len) {
      other.m_addr = static_cast<std::byte *>(MAP_FAILED);
      other.m_len = 0;
    }

    Memory_Map &operator=(Memory_Map &&other) noexcept {
      if (this != &other) {
        if (m_addr != MAP_FAILED) {
          munmap(m_addr, m_len);
        }

        m_addr = other.m_addr;
        m_len = other.m_len;

        other.m_addr = static_cast<std::byte *>(MAP_FAILED);
        other.m_len = 0;
      }
      return *this;
    }

    ~Memory_Map() {
      if (m_addr != MAP_FAILED) {
        munmap(m_addr, m_len);
      }
    }

    const std::byte *get_addr() const { return m_addr; }
    size_t get_len() const { return m_len; }

  private:
    std::byte *m_addr = static_cast<std::byte *>(MAP_FAILED);
    size_t m_len = 0;
  };
}