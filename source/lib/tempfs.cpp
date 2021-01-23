
#include "shm/tempfs.hpp"

#include <com/fatal_error.hpp>

#include "roster.hpp"

namespace miu::shm {

std::filesystem::path
tempfs::root() {
    return "/dev/shm";
}

void
tempfs::do_remove(std::filesystem::path path) {
    auto name = path.filename().string();
    if (roster::instance()->contains(name)) {
        FATAL_ERROR<std::logic_error>("shm file is still in use");
    }
    std::filesystem::remove(path);
}

}    // namespace miu::shm
