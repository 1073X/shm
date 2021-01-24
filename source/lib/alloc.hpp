#pragma once

#include <string_view>
#include <utility>    // std::pair

#include "head.hpp"

namespace miu::shm {

extern uint32_t align(uint32_t);
extern head* alloc(std::string, uint32_t);
extern void dealloc(head*);

}    // namespace miu::shm
