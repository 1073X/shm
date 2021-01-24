
#include "shm/buffer.hpp"

#include <com/predict.hpp>
#include <log/log.hpp>

#include "head.hpp"
#include "roster.hpp"

namespace miu::shm {

buffer::buffer(com::strcat const& name_cat, uint32_t len) noexcept {
    auto name = name_cat.str();
    if (!name.empty()) {
        if (name.size() < sizeof(head::name)) {
            if (roster::instance()->try_insert(name)) {
                load(name, len);
                if (!_head) {
                    roster::instance()->erase(name);
                }
            } else {
                log::error(+"duplicated shm", name);
            }
        } else {
            log::error(+"invalid shm name", name);
        }
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
        log::debug(+"release shm", name());
        roster::instance()->erase(name());    // earse anyway
        head::close(_head);
    }
}

bool buffer::operator!() const {
    return !_head;
}

std::string buffer::name() const {
    assert(_head != nullptr);
    return { +_head->name };
}

uint32_t buffer::size() {
    assert(_head != nullptr);
    if (UNLIKELY(_head->size > _size)) {
        load(name(), 0);
    }
    return _head->size;
}

char* buffer::addr() {
    assert(_head != nullptr);
    if (UNLIKELY(_head->size > _size)) {
        load(name(), 0);
    }
    return (char*)_head + _head->offset;
}

void buffer::resize(uint32_t new_size) {
    assert(_head != nullptr);
    if (new_size > size()) {
        load(name(), new_size);
    }
}

void buffer::load(std::string name, uint32_t size) {
    head::close(_head);
    if (size > 0) {
        _head = head::make(name, size);
    } else {
        _head = head::open(name);
    }

    if (_head) {
        _size = _head->size;
        log::debug(+"load shm", name, _head->size);
    }
}

}    // namespace miu::shm
