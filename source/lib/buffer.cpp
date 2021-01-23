
#include "shm/buffer.hpp"

#include <log/log.hpp>

#include "alloc.hpp"
#include "head.hpp"
#include "roster.hpp"

namespace miu::shm {

buffer::buffer(com::strcat const& name, uint32_t len) noexcept {
    if (roster::instance()->try_insert(name.str())) {
        _head = alloc(name.str(), align(len));
        if (!_head) {
            roster::instance()->erase(name.str());
        }
    } else {
        log::error(name.str(), +"create duplicated shm::buffer");
    }
}

buffer::buffer(buffer&& another)
    : _head(another._head) {
    another._head = nullptr;
}

buffer& buffer::operator=(buffer&& another) {
    std::swap(_head, another._head);
    return *this;
}

buffer::~buffer() {
    if (_head) {
        roster::instance()->erase(name());    // earse anyway
        dealloc(_head);
    }
}

bool buffer::operator!() const {
    return !_head;
}

const char* buffer::name() const {
    assert(_head != nullptr);
    return +_head->name;
}

uint32_t buffer::size() const {
    assert(_head != nullptr);
    return _head->size - sizeof(head);
}

const char* buffer::addr() const {
    assert(_head != nullptr);
    return (const char*)(_head + 1);
}

char* buffer::addr() {
    auto val = const_cast<buffer const*>(this)->addr();
    return const_cast<char*>(val);
}

}    // namespace miu::shm
