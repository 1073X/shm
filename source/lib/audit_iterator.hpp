#pragma once

#include "audit.hpp"

namespace miu::shm {

class audit_iterator {
  public:
    audit_iterator(audit* start, uint32_t idx, uint32_t cap)
        : _start(start)
        , _idx(idx)
        , _cap(cap) {}

    auto operator!=(audit_iterator const& rhs) const {
        return _start != rhs._start || _idx != rhs._idx;
    }
    auto operator==(audit_iterator const& rhs) const { return !operator!=(rhs); }

    auto operator->() { return _start + _idx % _cap; }
    auto& operator*() { return *operator->(); }

    auto operator++() {
        _idx++;
        return *this;
    }

    auto operator++(int32_t) {
        auto curr = *this;
        operator++();
        return curr;
    }

  private:
    audit* _start;
    uint32_t _idx;
    uint32_t _cap;
};

}    // namespace miu::shm
