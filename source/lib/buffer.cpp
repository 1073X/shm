
#include "shm/buffer.hpp"

#include <cstring>    // std::strncpy
#include <log/log.hpp>

#include "alloc.hpp"
#include "head.hpp"
#include "roster.hpp"

namespace miu::shm {

buffer::buffer(std::string_view name) noexcept {
    if (roster::instance()->try_insert(name)) {
        auto [size, addr] = alloc(name, 0);
        if (!addr) {
            roster::instance()->erase(name);
        } else {
            _addr = addr;
        }
    } else {
        log::error(name, +"open duplicated shm::buffer");
    }
}

buffer::buffer(std::string_view name, uint32_t len) noexcept {
    if (roster::instance()->try_insert(name)) {
        auto [size, addr] = alloc(name, align(len));
        if (!addr) {
            roster::instance()->erase(name);
        } else {
            _addr  = addr;
            auto h = new (_addr) head {};
            std::strncpy(h->name, name.data(), sizeof(h->name));
            h->size = size;
        }
    } else {
        log::error(name, +"create duplicated shm::buffer");
    }
}

buffer::~buffer() {
    roster::instance()->erase(name());    // earse anyway
    if (_addr) {
        dealloc(_addr, size());
    }
}

const char* buffer::name() const {
    if (_addr) {
        auto h = (head const*)_addr;
        return +h->name;
    }
    return +"NULL";
}

uint32_t buffer::size() const {
    if (_addr) {
        auto h = (head const*)_addr;
        return h->size - sizeof(head);
    }
    return 0;
}

const char* buffer::addr() const {
    if (_addr) {
        return _addr + sizeof(head);
    }
    return nullptr;
}

char* buffer::addr() {
    auto val = const_cast<buffer const*>(this)->addr();
    return const_cast<char*>(val);
}

}    // namespace miu::shm
