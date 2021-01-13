#pragma once

#include <com/strcat.hpp>
#include <filesystem>

namespace miu::shm {

class tmpfs {
  public:
    static std::filesystem::path root();

    template<typename... ARGS>
    static auto join(ARGS&&... args) {
        return root() / com::strcat { std::forward<ARGS>(args)... }.str();
    }
};

}    // namespace miu::shm
