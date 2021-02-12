#pragma once

#include <com/datetime.hpp>
#include <com/strcat.hpp>

namespace miu::shm {

class buffer_impl;

class buffer {
  public:
    buffer(com::strcat const& = "", uint32_t = 0) noexcept;
    buffer(buffer&&);
    buffer& operator=(buffer&&);
    ~buffer();

    bool operator!() const;
    operator bool() const { return !operator!(); }

    std::string name() const;

    auto size() const { return _size; }
    char* data();

    void resize(uint32_t);

  private:
    void load(std::string, uint32_t);

  private:
    buffer_impl* _impl { nullptr };
    uint32_t _size { 0 };
};

}    // namespace miu::shm
