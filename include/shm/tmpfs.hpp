#pragma once

#include <com/fatal_error.hpp>
#include <com/strcat.hpp>
#include <filesystem>

namespace miu::shm {

class tmpfs {
  public:
    static std::filesystem::path root();

    template<typename... ARGS>
    static auto join(ARGS&&... args) {
        auto filename = com::strcat { std::forward<ARGS>(args)... }.str();
        if (filename.empty()) {
            FATAL_ERROR("empty shmem file name");
        }
        return root() / filename;
    }
};

}    // namespace miu::shm
