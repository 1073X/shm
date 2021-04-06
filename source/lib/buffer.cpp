
#include "shm/buffer.hpp"

#include <com/predict.hpp>
#include <log/log.hpp>

#include "buffer_impl.hpp"
#include "factory.hpp"

namespace miu::shm {

namespace details {
    template<typename CB>
    static auto create(std::string const& name, CB const& ctor) {
        auto impl = g_impl_factory.create(name, ctor);
        if (!impl) {
            g_impl_factory.destory(name);
        }
        return impl;
    }
}    // namespace details

buffer::buffer(com::strcat const& name_cat, uint32_t size) noexcept
    : buffer() {
    assert(size > 0 && "why do you want to create a 0 shm buffer?");

    auto name = name_cat.str();

    _impl = details::create(name, [&]() { return buffer_impl::make(name, size); });
    if (_impl) {
        _size = _impl->size();
        _mode = mode::RDWR;
    }
}

buffer::buffer(com::strcat const& name_cat, enum mode mode) noexcept
    : buffer() {
    auto name = name_cat.str();

    _impl = details::create(name, [&]() { return buffer_impl::open(name, mode); });
    if (_impl) {
        _size = _impl->size();
        _mode = mode;
    }
}

buffer::buffer(buffer const& another) noexcept {
    operator=(another);
}

buffer& buffer::operator=(buffer const& another) noexcept {
    if (another) {
        auto name = another.name();
        auto mode = another.mode();

        _impl = details::create(name, [&]() { return buffer_impl::open(name, mode); });
        if (_impl) {
            _size = _impl->size();
            _mode = mode;
        }
    }
    return *this;
}

buffer::~buffer() {
    close();
}

void buffer::close() {
    if (_impl && g_impl_factory.destory(name())) {
        buffer_impl::close(_impl, _mode);
    }
    _impl = nullptr;
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

const char* buffer::data() const {
    return const_cast<buffer*>(this)->data();
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
