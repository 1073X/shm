
#include "shm/buffer.hpp"

#include <com/predict.hpp>
#include <log/log.hpp>

#include "head.hpp"
#include "roster.hpp"

#define HEAD ((head*)_addr)

namespace miu::shm {

buffer::buffer(com::strcat const& name_cat, uint32_t len) noexcept {
    auto name = name_cat.str();
    if (!name.empty()) {
        if (name.size() < sizeof(head::name)) {
            if (roster::instance()->try_insert(name)) {
                load(name, len);
                if (!_addr) {
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
    : _addr(another._addr) {
    another._addr = nullptr;
}

buffer& buffer::operator=(buffer&& another) {
    std::swap(_addr, another._addr);
    return *this;
}

buffer::~buffer() {
    if (_addr) {
        log::debug(+"release shm", name());
        roster::instance()->erase(name());    // earse anyway
        head::close(HEAD);
    }
}

bool buffer::operator!() const {
    return !_addr;
}

std::string buffer::name() const {
    assert(_addr != nullptr);
    return { +HEAD->name };
}

uint32_t buffer::size() {
    assert(_addr != nullptr);
    if (UNLIKELY(HEAD->size > _size)) {
        load(name(), 0);
    }
    return HEAD->size;
}

char* buffer::data() {
    assert(_addr != nullptr);
    if (UNLIKELY(HEAD->size > _size)) {
        load(name(), 0);
    }
    return (char*)_addr + HEAD->offset;
}

void buffer::resize(uint32_t new_size) {
    assert(_addr != nullptr);
    if (new_size > size()) {
        load(name(), new_size);
    }
}

void buffer::load(std::string name, uint32_t size) {
    head::close(HEAD);
    if (size > 0) {
        _addr = head::make(name, size);
    } else {
        _addr = head::open(name);
    }

    if (_addr) {
        _size = HEAD->size;
        log::debug(+"load shm", name, HEAD->size);
    }
}

}    // namespace miu::shm
