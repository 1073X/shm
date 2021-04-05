#include <gtest/gtest.h>

#include <fstream>

#include "shm/buffer.hpp"
#include "shm/tempfs.hpp"

using miu::shm::tempfs;

TEST(ut_tempfs, join) {
    EXPECT_EQ("/dev/shm/xyz.1.abc", tempfs::join("xyz", 1, "abc"));
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

TEST(ut_tempfs, find_files) {
    std::ofstream { tempfs::join("c", "txt") };
    std::ofstream { tempfs::join("a", "txt") };
    std::ofstream { tempfs::join("b", "txt") };
    std::ofstream { tempfs::join("d", "csv") };

    auto files = tempfs::find(".*\\.txt");
    EXPECT_EQ(3, files.size());
    EXPECT_EQ(tempfs::join("a.txt"), files[0]);
    EXPECT_EQ(tempfs::join("b.txt"), files[1]);
    EXPECT_EQ(tempfs::join("c.txt"), files[2]);

    EXPECT_EQ(0U, tempfs::find("not_exists").size());

    tempfs::remove("a", "txt");
    tempfs::remove("b", "txt");
    tempfs::remove("c", "txt");
    tempfs::remove("d", "csv");
}
