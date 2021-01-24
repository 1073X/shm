
#include "shm/buffer.hpp"

#include <com/predict.hpp>
#include <log/log.hpp>

#include "alloc.hpp"
#include "head.hpp"
#include "roster.hpp"

namespace miu::shm {

buffer::buffer(com::strcat const& name_cat, uint32_t len) noexcept {
    auto name = name_cat.str();
    if (!name.empty()) {
        if (name.size() < sizeof(head::name)) {
            if (roster::instance()->try_insert(name)) {
                _head = alloc(name, align(len));
                if (!_head) {
                    roster::instance()->erase(name);
                } else {
                    _size = _head->size;
                    log::debug((len > 0 ? +"create shm" : +"open shm"), name, _head->size);
                }
            } else {
                log::error(+"cann't create or open duplicated shm", name);
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
        dealloc(_head);
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
        resize(_head->size);
    }
    return _head->size;
}

char* buffer::addr() {
    assert(_head != nullptr);
    if (UNLIKELY(_head->size > _size)) {
        resize(_head->size);
    }
    return (char*)(_head + 1);
}

void buffer::resize(uint32_t new_size) {
    assert(_head != nullptr);
    auto name     = this->name();
    auto old_size = _head->size;
    if (new_size > old_size) {
        dealloc(_head);
        _head = alloc(name, new_size);
        if (_head) {
            _size = _head->size;
            log::debug(+"resize shm", name, old_size, +"->", _head->size);
        }
    }
}

}    // namespace miu::shm
