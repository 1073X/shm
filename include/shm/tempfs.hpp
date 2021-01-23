#pragma once

#include <com/fatal_error.hpp>
#include <com/strcat.hpp>
#include <filesystem>

namespace miu::shm {

class tempfs {
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

    template<typename... ARGS>
    static auto file_size(ARGS&&... args) {
        return std::filesystem::file_size(join(std::forward<ARGS>(args)...));
    }

    template<typename... ARGS>
    static auto exists(ARGS&&... args) {
        return std::filesystem::exists(join(std::forward<ARGS>(args)...));
    }

    template<typename... ARGS>
    static auto remove(ARGS&&... args) {
        return do_remove(join(std::forward<ARGS>(args)...));
    }

  private:
    static void do_remove(std::filesystem::path);
};

}    // namespace miu::shm
