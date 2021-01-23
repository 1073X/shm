#pragma once

#include <string_view>
#include <utility>    // std::pair

#include "head.hpp"

namespace miu::shm {

extern uint32_t align(uint32_t size);
extern head* alloc(std::string_view name, uint32_t size);
extern void dealloc(head*);

}    // namespace miu::shm
