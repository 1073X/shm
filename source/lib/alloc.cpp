
#include "alloc.hpp"

#include <fcntl.h>     /* For O_* constants */
#include <string.h>    // strerror
#include <sys/mman.h>
#include <sys/stat.h>  /* For mode constants */
#include <unistd.h>    // ftruncate

#include <cstring>    // std::strncpy
#include <log/log.hpp>

namespace miu::shm {

static uint32_t const PAGE_SIZE = getpagesize();

uint32_t align(uint32_t size) {
    return ((size + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
}

head* alloc(std::string name, uint32_t size) {
    size = ((size + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;

    auto flag = O_RDWR;
    if (size > 0) {
        flag |= O_CREAT;
    }

    // 1. open file
    auto fd = shm_open(name.c_str(), flag, 0);
    if (fd <= 0) {
        const char* err_str = strerror(errno);
        log::error(name, +"shm_open", errno, err_str);
        return nullptr;
    }

    // 2. verify and adjust file size
    struct stat st;
    if (fstat(fd, &st)) {
        const char* err_str = strerror(errno);
        log::error(name, +"fstat", errno, err_str);
        ::close(fd);
        return nullptr;
    }

    if (st.st_size < size) {
        if (ftruncate(fd, size)) {
            const char* err_str = strerror(errno);
            log::error(name, +"ftruncate", errno, err_str);
            ::close(fd);
            return nullptr;
        }
    } else {
        size = st.st_size;
        if (size < PAGE_SIZE) {
            log::error(name, +"size is less than ", PAGE_SIZE);
            ::close(fd);
            return nullptr;
        }
    }

    // 3. chmod 660 (since the default permission mode is 640)
    if (fchmod(fd, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) {
        const char* err_str = strerror(errno);
        log::warn(name, +"fchmod", errno, err_str);
    }

    // 4. map the file in memory
    /*******************************
       If addr is NULL, then the kernel chooses the (page-aligned)
       address at which to create the mapping; this is the most portable
       method of creating a new mapping.  If addr is not NULL, then the
       kernel takes it as a hint about where to place the mapping; on
       Linux, the kernel will pick a nearby page boundary (but always
       above or equal to the value specified by
       /proc/sys/vm/mmap_min_addr) and attempt to create the mapping
       there.  If another mapping already exists there, the kernel picks
       a new address that may or may not depend on the hint.  The
       address of the new mapping is returned as the result of the call.
     ******************************/
    auto addr = (char*)::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr) {
        auto h = new (addr) head {};
        std::strncpy(h->name, name.c_str(), sizeof(h->name));
        h->size = size - sizeof(head);
    } else {
        const char* err_str = strerror(errno);
        log::error(name, +"mmap", errno, err_str);
    }

    ::close(fd);
    return (head*)addr;
}

void dealloc(head* h) {
    if (h) {
        auto size = h->size + sizeof(head);
        msync(h, size, MS_SYNC | MS_INVALIDATE);
        munmap(h, size);
    }
}

}    // namespace miu::shm
