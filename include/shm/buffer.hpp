#pragma once

#include <cstdint>
#include <string>

namespace miu::shm {

class buffer {
  public:
    buffer() = default;
    buffer(std::string_view) noexcept;              // open
    buffer(std::string_view, uint32_t) noexcept;    // create
    buffer(buffer const&) = delete;
    auto operator=(buffer const&) = delete;
    ~buffer();

    auto operator!() const { return !_addr; }
    operator bool() const { return !operator!(); }

    const char* name() const;
    uint32_t size() const;
    const char* addr() const;
    char* addr();

  private:
    char* _addr { nullptr };
};

}    // namespace miu::shm
