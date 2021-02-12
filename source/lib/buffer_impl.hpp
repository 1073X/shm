#pragma once

#include <com/datetime.hpp>
#include <string>

#include "audit.hpp"

namespace miu::shm {

class buffer_impl {
  public:
    buffer_impl(std::string_view name, uint32_t total, uint32_t offset);

    static buffer_impl* make(std::string, uint32_t);
    static buffer_impl* open(std::string);
    static void close(buffer_impl*);

    const char* name() const { return _name; }
    auto size() const { return _size; }
    auto offset() const { return _offset; }

    auto audit_size() const { return _audit_size; }
    auto audits() const { return _audits; }

    void add_audit_log(std::string_view);

  private:
    char _name[16];
    uint32_t _size;
    uint32_t _offset;
    uint32_t _audit_size;
    uint32_t _padding[9];
    audit _audits[];
};
static_assert(CACHE_LINE == sizeof(buffer_impl));

}    // namespace miu::shm
