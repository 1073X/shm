#include <gtest/gtest.h>

#include "shm/tmpfs.hpp"

TEST(ut_tmpfs, join) { EXPECT_EQ("/dev/shm/xyz.1.abc", miu::shm::tmpfs::join("xyz", 1, "abc")); }
