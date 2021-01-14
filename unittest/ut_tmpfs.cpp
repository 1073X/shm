#include <gtest/gtest.h>

#include <fstream>

#include "shm/tempfs.hpp"

using miu::shm::tempfs;

TEST(ut_tempfs, join) {
    EXPECT_EQ("/dev/shm/xyz.1.abc", tempfs::join("xyz", 1, "abc"));
    EXPECT_ANY_THROW(tempfs::join());
    EXPECT_ANY_THROW(tempfs::join(""));
}

TEST(ut_tempfs, exists_and_remove) {
    EXPECT_FALSE(tempfs::exists("ut_tempfs", 1));
    std::ofstream { tempfs::join("ut_tempfs", 1) };
    EXPECT_TRUE(tempfs::exists("ut_tempfs", 1));
    tempfs::remove("ut_tempfs", 1);
    EXPECT_FALSE(tempfs::exists("ut_tempfs", 1));
}
