#include <gtest/gtest.h>

#include <iostream>

namespace miu::shm {
extern std::string_view version();
extern std::string_view build_info();
}    // namespace miu::shm

TEST(ut_version, version) {
    std::cout << miu::shm::version() << std::endl;
    std::cout << miu::shm::build_info() << std::endl;
}
