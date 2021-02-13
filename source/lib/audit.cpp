
#include "audit.hpp"

#include <iomanip>
#include <meta/info.hpp>
#include <sstream>

namespace miu::shm {

audit::audit(std::string_view text) {
    _time = com::datetime::now();
    std::strncpy(_info, meta::info(), sizeof(_info));

    assert(sizeof(_text) > text.size() && "text overflow");
    std::strncpy(_text, text.data(), sizeof(_text));
}

}    // namespace miu::shm

DEF_TO_STRING(miu::shm::audit) {
    std::ostringstream ss;
    ss << '[' << to_string(v.time()) << ']';
    ss << ' ' << std::setw(7) << std::setfill(' ') << v.text();
    ss << ' ' << v.info();
    return ss.str();
}
