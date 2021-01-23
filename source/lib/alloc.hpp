#pragma once

#include <string_view>
#include <utility>    // std::pair

namespace miu::shm {

extern uint32_t align(uint32_t size);
extern std::pair<uint32_t, char*> alloc(std::string_view name, uint32_t size);
extern void dealloc(char* addr, uint32_t size);

}    // namespace miu::shm
