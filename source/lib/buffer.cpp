
#include "shm/buffer.hpp"

#include <log/log.hpp>

#include "alloc.hpp"
#include "head.hpp"
#include "roster.hpp"

namespace miu::shm {

buffer::buffer(std::string_view name, uint32_t len) noexcept {
    if (roster::instance()->try_insert(name)) {
        _head = alloc(name, align(len));
        if (!_head) {
            roster::instance()->erase(name);
        }
    } else {
        log::error(name, +"create duplicated shm::buffer");
    }
}

buffer::~buffer() {
    roster::instance()->erase(name());    // earse anyway
    dealloc(_head);
}

bool buffer::operator!() const {
    return !_head;
}

const char* buffer::name() const {
    if (_head) {
        return +_head->name;
    }
    return +"NULL";
}

uint32_t buffer::size() const {
    if (_head) {
        return _head->size - sizeof(head);
    }
    return 0;
}

const char* buffer::addr() const {
    if (_head) {
        return (const char*)(_head + 1);
    }
    return nullptr;
}

char* buffer::addr() {
    auto val = const_cast<buffer const*>(this)->addr();
    return const_cast<char*>(val);
}

}    // namespace miu::shm
