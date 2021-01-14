#pragma once

#include <mutex>
#include <set>

namespace miu::shm {

class roster {
  public:
    auto try_insert(std::string_view name) {
        std::lock_guard<std::mutex> l { mtx };
        return names.insert(name.data()).second;
    }

    auto erase(std::string_view name) {
        std::lock_guard<std::mutex> l { mtx };
        names.erase(name.data());
    }

  private:
    std::mutex mtx;
    std::set<std::string> names;
};

}    // namespace miu::shm
