
#include "head.hpp"

#include <fcntl.h>     /* For O_* constants */
#include <string.h>    // strerror
#include <sys/mman.h>
#include <sys/stat.h>  /* For mode constants */
#include <unistd.h>    // ftruncate

#include <cstring>    // std::strncpy
#include <log/log.hpp>

#define ERR_RETURN(FUNC)                                           \
    log::error(name, +#FUNC, errno, (const char*)strerror(errno)); \
    ::close(fd);                                                   \
    return nullptr

namespace miu::shm {

static uint32_t const PAGE_SIZE = getpagesize();

head* head::make(std::string name, uint32_t size) {
    // 1. open file
    auto fd = shm_open(name.c_str(), O_RDWR | O_CREAT, 0);
    if (fd <= 0) {
        ERR_RETURN(shm_open);
    }

    // 2. verify and adjust file size
    struct stat st;
    if (fstat(fd, &st)) {
        ERR_RETURN(fstat);
    }

    auto resize_time = com::datetime::min();
    auto total      = ((size + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE + PAGE_SIZE;
    if (st.st_size < total) {
        if (ftruncate(fd, total)) {
            ERR_RETURN(ftruncate);
        }
        resize_time = com::datetime::now();
    } else {
        total = st.st_size;
    }

    // 3. chmod 660 (since the default permission mode is 640)
    if (fchmod(fd, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) {
        ERR_RETURN(fchmod);
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
    auto addr = (char*)::mmap(nullptr, total, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (!addr) {
        ERR_RETURN(mmap);
    }

    auto h = new (addr) head {};
    std::strncpy(h->name, name.c_str(), sizeof(h->name));
    h->offset     = PAGE_SIZE;
    h->size       = total - h->offset;
    h->resize_time = std::max(resize_time, h->resize_time);

    ::close(fd);
    return (head*)addr;
}

head* head::open(std::string name) {
    // 1. open file
    auto fd = shm_open(name.c_str(), O_RDWR, 0);
    if (fd <= 0) {
        ERR_RETURN(shm_open);
    }

    // 2. verify and adjust file size
    struct stat st;
    if (fstat(fd, &st)) {
        ERR_RETURN(fstat);
    }

    auto total = st.st_size;
    if (total < PAGE_SIZE) {
        ERR_RETURN(LESS_THAN_PAGE_SIZE);
    }

    // 3. map the file in memory
    auto addr = (char*)::mmap(nullptr, total, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (!addr) {
        ERR_RETURN(mmap);
    }

    ::close(fd);
    return (head*)addr;
}

void head::close(head* h) {
    if (h) {
        auto total = h->size + h->offset;
        msync(h, total, MS_SYNC | MS_INVALIDATE);
        munmap(h, total);
    }
}

}    // namespace miu::shm
