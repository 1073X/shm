#pragma once

#include <com/strcat.hpp>

namespace miu::shm {

class head;

class buffer {
  public:
    buffer(com::strcat const& name = "", uint32_t len = 0) noexcept;
    buffer(buffer&&);
    buffer& operator=(buffer&&);
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
