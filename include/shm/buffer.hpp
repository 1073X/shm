#pragma once

#include <com/strcat.hpp>

#include "mode.hpp"

namespace miu::shm {

class buffer_impl;

class buffer final {
  public:
    buffer() = default;
    buffer(com::strcat const&, uint32_t) noexcept;     // create or resize
    buffer(com::strcat const&, enum mode) noexcept;    // open read-only or read-write
    buffer(buffer const&) noexcept;
    buffer& operator=(buffer const&) noexcept;
    ~buffer();

    bool operator!() const;
    operator bool() const { return !operator!(); }

    std::string name() const;

    auto mode() const { return _mode; }
    auto size() const { return _size; }
    const char* data() const;
    char* data();

    bool resize(uint32_t);

  private:
    buffer_impl* _impl { nullptr };
    uint32_t _size { 0 };
    enum mode _mode { mode::MAX };
};

}    // namespace miu::shm
