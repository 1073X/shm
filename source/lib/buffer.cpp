
#include "shm/buffer.hpp"

#include <fcntl.h>     /* For O_* constants */
#include <string.h>    // strerror
#include <sys/mman.h>
#include <sys/stat.h>  /* For mode constants */
#include <unistd.h>    // ftruncate

#include <com/fatal_error.hpp>
#include <log/log.hpp>

namespace miu::shm {

static uint32_t const PAGE_SIZE = getpagesize();

static std::pair<uint32_t, char*>
alloc(std::string_view name, uint32_t size) {
    auto flag = O_RDWR;
    if (size > 0) {
        flag |= O_CREAT;
    }

    // 1. open file
    auto fd = shm_open(name.data(), flag, 0);
    if (fd <= 0) {
        const char* err_str = strerror(errno);
        log::error(name, +"shm_open", errno, err_str);
        return std::make_pair(0, nullptr);
    }

    // 2. verify and adjust file size
    struct stat st;
    if (fstat(fd, &st)) {
        const char* err_str = strerror(errno);
        log::error(name, +"fstat", errno, err_str);
        ::close(fd);
        return std::make_pair(0, nullptr);
    }

    if (st.st_size < size) {
        if (ftruncate(fd, size)) {
            const char* err_str = strerror(errno);
            log::error(name, +"ftruncate", errno, err_str);
            ::close(fd);
            return std::make_pair(0, nullptr);
        }
    } else {
        size = st.st_size;
        if (size < PAGE_SIZE) {
            log::error(name, +"size is less than ", PAGE_SIZE);
            ::close(fd);
            return std::make_pair(0, nullptr);
        }
    }

    // 3. chmod 660 (since the default permission mode is 640)
    if (fchmod(fd, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) {
        const char* err_str = strerror(errno);
        log::warn(name, +"fchmod", errno, err_str);
    }

    // 4. map the file in memory
    auto addr = (char*)::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (!addr) {
        const char* err_str = strerror(errno);
        log::error(name, +"mmap", errno, err_str);
    }

    ::close(fd);
    return std::make_pair(size, addr);
}

buffer::buffer(std::string_view name) noexcept { std::tie(_size, _addr) = alloc(name, 0); }

buffer::buffer(std::string_view name, uint32_t size) noexcept {
    std::tie(_size, _addr) = alloc(name, ((size + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE);
}

buffer::~buffer() {
    if (_addr) {
        msync(_addr, _size, MS_SYNC | MS_INVALIDATE);
        munmap(_addr, _size);
    }
}

}    // namespace miu::shm
