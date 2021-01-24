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

    void resize(uint32_t);

    std::string name() const;
    uint32_t size();
    char* addr();

  private:
    head* _head { nullptr };
    uint32_t _size { 0 };
};

}    // namespace miu::shm
