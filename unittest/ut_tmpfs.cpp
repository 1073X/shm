#include <gtest/gtest.h>

#include "shm/tmpfs.hpp"

using miu::shm::tmpfs;

TEST(ut_tmpfs, join) {
    EXPECT_EQ("/dev/shm/xyz.1.abc", tmpfs::join("xyz", 1, "abc"));
    EXPECT_ANY_THROW(tmpfs::join());
    EXPECT_ANY_THROW(tmpfs::join(""));
}
