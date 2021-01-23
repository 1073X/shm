#pragma once

namespace miu::shm {

struct head {
    char name[16];
    uint32_t size;
    char padding[44];
};
static_assert(CACHE_LINE == sizeof(head));

}    // namespace miu::shm
