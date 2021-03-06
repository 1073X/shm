#pragma once

#include <cassert>
#include <com/to_string.hpp>
#include <cstring>    // std::strncpy
#include <time/stamp.hpp>

namespace miu::shm {

class audit {
  public:
    explicit audit(std::string_view text);

    std::string_view info() const { return _info; }
    std::string_view text() const { return _text; }
    auto time() const { return _time; }

  private:
    char _info[48];
    char _text[8];
    time::stamp _time;
};
static_assert(sizeof(audit) == CACHE_LINE);

}    // namespace miu::shm

DEF_TO_STRING(miu::shm::audit);
