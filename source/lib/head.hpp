#pragma once

#include <com/datetime.hpp>
#include <string>

namespace miu::shm {

struct head {
    char name[16];
    uint32_t size;
    uint32_t offset;
    com::datetime resize_time;
    char padding[32];

    static head* make(std::string, uint32_t);
    static head* open(std::string);
    static void close(head*);
};
static_assert(CACHE_LINE == sizeof(head));

}    // namespace miu::shm
