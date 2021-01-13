#include <gtest/gtest.h>

#include <fstream>

#include "shm/tmpfs.hpp"

using miu::shm::tmpfs;

TEST(ut_tmpfs, join) {
    EXPECT_EQ("/dev/shm/xyz.1.abc", tmpfs::join("xyz", 1, "abc"));
    EXPECT_ANY_THROW(tmpfs::join());
    EXPECT_ANY_THROW(tmpfs::join(""));
}

TEST(ut_tmpfs, exists_and_remove) {
    EXPECT_FALSE(tmpfs::exists("ut_tmpfs", 1));
    std::ofstream { tmpfs::join("ut_tmpfs", 1) };
    EXPECT_TRUE(tmpfs::exists("ut_tmpfs", 1));
    tmpfs::remove("ut_tmpfs", 1);
    EXPECT_FALSE(tmpfs::exists("ut_tmpfs", 1));
}
