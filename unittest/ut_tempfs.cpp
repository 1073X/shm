#include <gtest/gtest.h>

#include <fstream>

#include "shm/buffer.hpp"
#include "shm/tempfs.hpp"

using miu::shm::tempfs;

TEST(ut_tempfs, join) {
    EXPECT_EQ("/dev/shm/xyz.1.abc", tempfs::join("xyz", 1, "abc"));
    EXPECT_ANY_THROW(tempfs::join());
    EXPECT_ANY_THROW(tempfs::join(""));
}

TEST(ut_tempfs, exists_and_remove) {
    {
        miu::shm::buffer buf { "ut_tempfs", 4096 };
        EXPECT_ANY_THROW(tempfs::remove("ut_tempfs"));
    }

    EXPECT_TRUE(tempfs::exists("ut_tempfs"));
    EXPECT_NO_THROW(tempfs::remove("ut_tempfs"));
    EXPECT_FALSE(tempfs::exists("ut_tempfs"));
}
