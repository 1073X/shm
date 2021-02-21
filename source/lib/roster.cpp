
#include "roster.hpp"

#include <log/log.hpp>

namespace miu::shm {

roster roster::inst;

bool roster::try_insert(std::string_view name) {
    if (name.empty()) {
        log::error(+"shm EMPTY NAME");
        return false;
    }

    std::lock_guard<std::mutex> l { _mtx };
    if (!_names.insert(name.data()).second) {
        log::error(+"shm", name, +"DUPLICATED");
        return false;
    }

    return true;
}

void roster::erase(std::string_view name) {
    std::lock_guard<std::mutex> l { _mtx };
    _names.erase(name.data());
}

bool roster::contains(std::string_view name) {
    std::lock_guard<std::mutex> l { _mtx };
    return _names.end() != _names.find(name.data());
}

}    // namespace miu::shm
