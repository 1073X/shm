#pragma once

#include <sys/file.h>    // flock

namespace miu::shm {

class file_lock {
  public:
    explicit file_lock(int32_t fd)
        : _fd(fd) {
        ::flock(_fd, LOCK_EX);
    }

    ~file_lock() { ::flock(_fd, LOCK_UN); }

  private:
    int32_t _fd;
};

}    // namespace miu::shm
