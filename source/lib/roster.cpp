
#include "roster.hpp"

namespace miu::shm {

roster roster::_instance;

bool roster::try_insert(std::string_view name) {
    std::lock_guard<std::mutex> l { _mtx };
    return _names.insert(name.data()).second;
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
