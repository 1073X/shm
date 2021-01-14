#pragma once

#include <mutex>
#include <set>

namespace miu::shm {

class roster {
  public:
    auto try_insert(std::string_view name) {
        std::lock_guard<std::mutex> l { _mtx };
        return _names.insert(name.data()).second;
    }

    auto erase(std::string_view name) {
        std::lock_guard<std::mutex> l { _mtx };
        _names.erase(name.data());
    }

  private:
    std::mutex _mtx;
    std::set<std::string> _names;
};

}    // namespace miu::shm
