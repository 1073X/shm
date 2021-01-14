
#include "shm/tempfs.hpp"

namespace miu::shm {

std::filesystem::path
tempfs::root() {
    return "/dev/shm";
}

}    // namespace miu::shm
