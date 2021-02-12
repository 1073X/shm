#pragma once

#include <cassert>
#include <com/datetime.hpp>
#include <cstring>    // std::strncpy
#include <meta/info.hpp>

namespace miu::shm {

class audit {
  public:
    audit(std::string_view text) {
        _time = com::datetime::now();
        std::strncpy(_cate, meta::category(), sizeof(_cate));
        std::strncpy(_type, meta::type(), sizeof(_type));
        std::strncpy(_name, meta::name(), sizeof(_name));

        assert(sizeof(_text) > text.size() && "text overflow");
        std::strncpy(_text, text.data(), sizeof(_text));
    }

    std::string_view category() const { return _cate; }
    std::string_view type() const { return _type; }
    std::string_view name() const { return _name; }
    std::string_view text() const { return _text; }
    auto time() const { return _time; }

  private:
    char _cate[16];
    char _type[16];
    char _name[16];
    char _text[8];
    com::datetime _time;
};
static_assert(sizeof(audit) == CACHE_LINE);

}    // namespace miu::shm
