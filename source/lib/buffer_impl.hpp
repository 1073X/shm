#pragma once

#include <string>

#include "shm/mode.hpp"

#include "audit_iterator.hpp"

namespace miu::shm {

class buffer_impl {
  public:
    buffer_impl(std::string_view name, uint32_t total, uint32_t offset);

    static buffer_impl* make(std::string, uint32_t);
    static buffer_impl* open(std::string, mode);
    static void close(buffer_impl*, mode);

    const char* name() const { return _name; }
    auto size() const { return _size; }
    auto offset() const { return _offset; }

    uint32_t audit_max() const;
    auto audit_size() const { return _audit_size; }

    audit_iterator begin();
    audit_iterator end();

    void add_audit(int32_t, std::string_view);

  private:
    auto padding() const { return _padding; }

  private:
    char _name[32];
    uint32_t _size;
    uint32_t _offset;
    uint32_t _audit_size;
    uint32_t _padding[5];
    audit _audits[];
};
static_assert(CACHE_LINE == sizeof(buffer_impl));

}    // namespace miu::shm
