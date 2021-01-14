#pragma once

#include <cstdint>
#include <string_view>

namespace miu::shm {

class buffer {
  public:
    buffer() = default;
    buffer(std::string_view) noexcept;                   // open
    buffer(std::string_view, uint32_t size) noexcept;    // create
    ~buffer();

    auto operator!() const { return !_addr; }
    operator bool() const { return !operator!(); }

    auto size() const { return _size; }
    auto addr() const { return _addr; }

  private:
    uint32_t _size { 0 };
    char* _addr { nullptr };
};

}    // namespace miu::shm
