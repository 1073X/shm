#pragma once

#include <log/log.hpp>
#include <map>
#include <mutex>

namespace miu::shm {

template<typename T>
class factory {
  public:
    ~factory() {
        std::lock_guard<std::mutex> l { _mtx };
        for (auto [name, item] : _map) {
            log::warn(+"shm FACTORY", name, +"was not destoried REF", item.first);
        }
    }

    auto contains(std::string const& name) const {
        std::lock_guard<std::mutex> l { _mtx };
        return _map.end() != _map.find(name);
    }

    template<typename CTOR>
    auto create(std::string const& name, CTOR const& ctor) {
        std::lock_guard<std::mutex> l { _mtx };
        auto it = _map.find(name);
        if (it == _map.end()) {
            it = _map.emplace(name, std::make_pair(0, ctor())).first;
        }

        it->second.first++;
        return it->second.second;
    }

    auto destory(std::string const& name) {
        std::lock_guard<std::mutex> l { _mtx };
        auto it = _map.find(name);
        if (_map.end() != it && !--it->second.first) {
            log::debug(+"shm FACTORY erase", name);
            _map.erase(it);
            return true;
        }
        return false;
    }

  private:
    mutable std::mutex _mtx;
    std::map<std::string, std::pair<uint32_t, T>> _map;
};

class buffer_impl;
extern factory<buffer_impl*> g_impl_factory;

}    // namespace miu::shm
