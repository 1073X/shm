
#include "shm/tempfs.hpp"

namespace miu::shm {

com::directory tempfs { "/dev/shm" };    // NOLINT

}    // namespace miu::shm
