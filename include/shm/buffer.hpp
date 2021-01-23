#pragma once

#include <cstdint>
#include <string>

namespace miu::shm {

class head;

class buffer {
  public:
    buffer() = default;
    buffer(std::string_view, uint32_t = 0) noexcept;
    buffer(buffer const&) = delete;
    auto operator=(buffer const&) = delete;
    ~buffer();

    bool operator!() const;
    operator bool() const { return !operator!(); }

    const char* name() const;
    uint32_t size() const;
    const char* addr() const;
    char* addr();

  private:
    head* _head { nullptr };
};

}    // namespace miu::shm
