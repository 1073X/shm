
#include "buffer_impl.hpp"

#include <fcntl.h>     /* For O_* constants */
#include <string.h>    // strerror
#include <sys/mman.h>
#include <sys/stat.h>  /* For mode constants */
#include <unistd.h>    // ftruncate

#include <cstring>    // std::strncpy
#include <log/log.hpp>

#include "file_lock.hpp"

#define ERR_RETURN(FUNC)                                                   \
    log::error(+"shm", name, +#FUNC, errno, (const char*)strerror(errno)); \
    ::close(fd);                                                           \
    return nullptr

namespace miu::shm {

static uint32_t const PAGE_SIZE = getpagesize();

buffer_impl* buffer_impl::make(std::string name, uint32_t size) {
    // 0. check name length
    if (name.size() >= sizeof(buffer_impl::_name)) {
        log::error(+"shm", name, +"excceeds max shm name length");
        return nullptr;
    }

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

    auto total = ((size + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE + PAGE_SIZE;
    if (st.st_size < total) {
        if (ftruncate(fd, total)) {
            ERR_RETURN(ftruncate);
        }
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
    auto addr = ::mmap(nullptr, total, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (!addr) {
        ERR_RETURN(mmap);
    }

    auto impl = new (addr) buffer_impl { name, total, PAGE_SIZE };

    // 5. audit
    impl->add_audit(fd, "MAKE");

    ::close(fd);
    return impl;
}

buffer_impl* buffer_impl::open(std::string name) {
    // 1. open file
    auto fd = shm_open(name.c_str(), O_RDWR, 0);
    if (fd <= 0) {
        ERR_RETURN(shm_open);
    }

    // 2. retrieve total size
    struct stat st;
    if (fstat(fd, &st)) {
        ERR_RETURN(fstat);
    }

    auto total = st.st_size;
    if (total < PAGE_SIZE) {
        ERR_RETURN(LESS_THAN_PAGE_SIZE);
    }

    // 3. map the file in memory
    auto impl = (buffer_impl*)::mmap(nullptr, total, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (!impl) {
        ERR_RETURN(mmap);
    }

    // 4. audit
    impl->add_audit(fd, "OPEN");

    ::close(fd);
    return impl;
}

void buffer_impl::close(buffer_impl* impl) {
    if (impl) {
        auto fd = shm_open(impl->name(), O_RDONLY, 0);
        impl->add_audit(fd, "CLOSE");
        ::close(fd);

        auto total = impl->_size + impl->_offset;
        msync(impl, total, MS_SYNC | MS_INVALIDATE);
        munmap(impl, total);
    }
}

///////////////////////////////////////////////////////////////

buffer_impl::buffer_impl(std::string_view name, uint32_t total, uint32_t offset) {
    std::strncpy(_name, name.data(), sizeof(_name));
    _offset = offset;
    _size   = total - offset;
}

uint32_t buffer_impl::audit_max() const {
    return (_offset - sizeof(buffer_impl)) / sizeof(audit);
}

void buffer_impl::add_audit(int32_t fd, std::string_view text) {
    file_lock lock { fd };

    auto idx = _audit_size++ % audit_max();
    new (_audits + idx) audit { text };
    log::debug(+"shm", std::string(name()), text, +"size =", size());
}

audit_iterator buffer_impl::begin() {
    auto idx = _audit_size >= audit_max() ? (_audit_size - audit_max()) % audit_max() : 0;
    return { _audits, idx, audit_max() };
}

audit_iterator buffer_impl::end() {
    return { _audits, _audit_size, audit_max() };
}

}    // namespace miu::shm
