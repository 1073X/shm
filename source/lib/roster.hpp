#pragma once

#include <mutex>
#include <set>

namespace miu::shm {

class roster {
  public:
    static auto instance() { return &inst; }

    bool try_insert(std::string_view name);
    void erase(std::string_view name);
    bool contains(std::string_view name);

  private:
    std::mutex _mtx;
    std::set<std::string> _names;

    static roster inst;
};

}    // namespace miu::shm
