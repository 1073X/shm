
#include "shm/tmpfs.hpp"

namespace miu::shm {

std::filesystem::path
tmpfs::root() {
    return "/dev/shm";
}

}    // namespace miu::shm
