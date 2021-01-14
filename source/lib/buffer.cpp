
#include "shm/buffer.hpp"

#include <fcntl.h>     /* For O_* constants */
#include <string.h>    // strerror
#include <sys/mman.h>
#include <sys/stat.h>  /* For mode constants */
#include <unistd.h>    // ftruncate

#include <com/fatal_error.hpp>
#include <log/log.hpp>

namespace miu::shm {

static char*
alloc(std::string_view name, uint32_t size) {
    auto fd = shm_open(name.data(), O_RDWR | O_CREAT, 0);
    if (fd <= 0) {
        const char* err_str = strerror(errno);
        log::error(name, +"shm_open", errno, err_str);
        return nullptr;
    }

    auto err = ftruncate(fd, size);
    if (err) {
        const char* err_str = strerror(errno);
        log::error(name, +"ftruncate", errno, err_str);
        ::close(fd);
        return nullptr;
    }

    err = fchmod(fd, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (err) {
        const char* err_str = strerror(errno);
        log::warn(name, +"fchmod", errno, err_str);
    }

    auto addr = (char*)::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (!addr) {
        const char* err_str = strerror(errno);
        log::error(name, +"mmap", errno, err_str);
    }

    ::close(fd);
    return addr;
}

buffer::buffer(std::string_view name, uint32_t size)
    : _size(size)
    , _addr(alloc(name, size)) {}

buffer::~buffer() {
    if (_addr) {
        msync(_addr, _size, MS_SYNC | MS_INVALIDATE);
        munmap(_addr, _size);
    }
}

}    // namespace miu::shm
