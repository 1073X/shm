#pragma once

#include <com/fatal_error.hpp>
#include <com/strcat.hpp>
#include <filesystem>
#include <vector>

namespace miu::shm {

class tempfs {
  public:
    static std::filesystem::path root();

    template<typename T, typename... ARGS>
    static auto join(T const& t, ARGS&&... args) {
        auto filename = com::strcat { t, std::forward<ARGS>(args)... }.str();
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

    static std::vector<std::filesystem::path> find(std::string_view filter);

  private:
    static void do_remove(std::filesystem::path);
};

}    // namespace miu::shm
