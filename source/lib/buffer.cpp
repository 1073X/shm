
#include "shm/buffer.hpp"

#include <com/predict.hpp>
#include <log/log.hpp>

#include "buffer_impl.hpp"
#include "roster.hpp"

namespace miu::shm {

buffer::buffer(com::strcat const& name_cat, uint32_t len) noexcept {
    auto name = name_cat.str();
    if (roster::instance()->try_insert(name)) {
        load(name, len);
        if (!_impl) {
            roster::instance()->erase(name);
        }
    }
}

buffer::buffer(buffer&& another)
    : _impl(another._impl) {
    another._impl = nullptr;
}

buffer& buffer::operator=(buffer&& another) {
    std::swap(_impl, another._impl);
    return *this;
}

buffer::~buffer() {
    if (_impl) {
        log::debug(+"release shm", name());
        roster::instance()->erase(name());    // erase anyway
        buffer_impl::close(_impl);
    }
}

bool buffer::operator!() const {
    return !_impl;
}

std::string buffer::name() const {
    assert(_impl != nullptr);
    return { +_impl->name() };
}

char* buffer::data() {
    assert(_impl != nullptr);
    if (UNLIKELY(_impl->size() > _size)) {
        load(name(), 0);
    }
    return (char*)_impl + _impl->offset();
}

void buffer::resize(uint32_t new_size) {
    assert(_impl != nullptr);
    if (new_size > _impl->size()) {
        load(name(), new_size);
    }
}

void buffer::load(std::string name, uint32_t size) {
    buffer_impl::close(_impl);
    _size = 0;

    if (size > 0) {
        _impl = buffer_impl::make(name, size);
    } else {
        _impl = buffer_impl::open(name);
    }

    if (_impl) {
        _size = _impl->size();
    }
}

}    // namespace miu::shm
