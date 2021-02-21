
#include "shm/buffer.hpp"

#include <com/predict.hpp>
#include <log/log.hpp>

#include "buffer_impl.hpp"
#include "roster.hpp"

namespace miu::shm {

buffer::buffer(com::strcat const& name_cat, uint32_t size) noexcept
    : buffer() {
    assert(size > 0 && "why do you want to create a 0 shm buffer?");

    auto name = name_cat.str();
    if (roster::instance()->try_insert(name)) {
        _impl = buffer_impl::make(name, size);
        if (!_impl) {
            roster::instance()->erase(name);
        } else {
            _size = _impl->size();
        }
    }
}

buffer::buffer(com::strcat const& name_cat) noexcept
    : buffer() {
    auto name = name_cat.str();
    if (roster::instance()->try_insert(name)) {
        _impl = buffer_impl::open(name);
        if (!_impl) {
            roster::instance()->erase(name);
        } else {
            _size = _impl->size();
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

    // TBD: this branch is not covered by unittest, because it is impossible to open the same
    // shm more than once in the same proc
    if (UNLIKELY(_impl->size() > _size)) {
        std::string name { this->name() };
        log::warn(+"shm", name, +"MUTATED", _size, +"->", _impl->size());

        buffer_impl::close(_impl);
        _impl = buffer_impl::open(name);
        _size = _impl->size();
    }

    return (char*)_impl + _impl->offset();
}

void buffer::resize(uint32_t new_size) {
    assert(_impl != nullptr);
    if (new_size > _impl->size()) {
        std::string name { this->name() };

        buffer_impl::close(_impl);
        _impl = buffer_impl::make(name, new_size);
        _size = _impl->size();
    }
}

}    // namespace miu::shm
