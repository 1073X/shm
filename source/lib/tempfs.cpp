
#include "shm/tempfs.hpp"

#include <com/fatal_error.hpp>
#include <regex>

#include "factory.hpp"

namespace miu::shm {

std::filesystem::path tempfs::root() {
    return "/dev/shm";
}

void tempfs::do_remove(std::filesystem::path path) {
    auto name = path.filename().string();
    if (g_impl_factory.contains(name)) {
        FATAL_ERROR<std::logic_error>("shm file is still in use");
    }
    std::filesystem::remove(path);
}

std::vector<std::filesystem::path> tempfs::find(std::string_view filter) {
    std::regex exp { filter.data(), filter.size() };

    std::vector<std::filesystem::path> files;
    for (const auto& entry : std::filesystem::directory_iterator(root())) {
        auto fname = entry.path().filename().string();
        std::smatch m;
        if (std::regex_match(fname, m, exp)) {
            files.push_back(entry);
        }
    }

    std::sort(files.begin(), files.end());
    return files;
}

}    // namespace miu::shm
