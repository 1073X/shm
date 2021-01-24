#pragma once

#include <com/strcat.hpp>

namespace miu::shm {

class buffer {
  public:
    buffer(com::strcat const& = "", uint32_t = 0) noexcept;
    buffer(buffer&&);
    buffer& operator=(buffer&&);
    ~buffer();

    bool operator!() const;
    operator bool() const { return !operator!(); }

    std::string name() const;
    uint32_t size();
    char* data();

    void resize(uint32_t);

  private:
    void load(std::string, uint32_t);

  private:
    void* _addr { nullptr };
    uint32_t _size { 0 };
};

}    // namespace miu::shm
