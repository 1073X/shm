
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
            _mode = mode::RDWR;
        }
    }
}

buffer::buffer(com::strcat const& name_cat, enum mode mode) noexcept
    : buffer() {
    auto name = name_cat.str();
    if (roster::instance()->try_insert(name)) {
        _impl = buffer_impl::open(name, mode);
        if (!_impl) {
            roster::instance()->erase(name);
        } else {
            _size = _impl->size();
            _mode = mode;
        }
    }
}

buffer::buffer(buffer&& another)
    : _impl(another._impl)
    , _size(another._size)
    , _mode(another._mode) {
    another._impl = nullptr;
    another._size = 0;
    another._mode = mode::MAX;
}

buffer& buffer::operator=(buffer&& another) {
    std::swap(_impl, another._impl);
    std::swap(_size, another._size);
    std::swap(_mode, another._mode);
    return *this;
}

buffer::~buffer() {
    if (_impl) {
        roster::instance()->erase(name());    // erase anyway
        buffer_impl::close(_impl, _mode);
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
    if (UNLIKELY(_impl->size() > _size && mode::RDWR == _mode)) {
        std::string name { this->name() };
        log::warn(+"shm", name, +"MUTATED", _size, +"->", _impl->size());

        buffer_impl::close(_impl, _mode);
        _impl = buffer_impl::open(name, _mode);
        _size = _impl->size();
    }

    return (char*)_impl + _impl->offset();
}

bool buffer::resize(uint32_t new_size) {
    if (mode::RDWR != _mode) {
        return false;
    }

    if (new_size > _impl->size()) {
        std::string name { this->name() };

        buffer_impl::close(_impl, _mode);
        _impl = buffer_impl::make(name, new_size);
        _size = _impl->size();
    }
    return true;
}

}    // namespace miu::shm
