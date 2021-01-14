#include <gtest/gtest.h>

#include <filesystem>

#include "shm/buffer.hpp"
#include "shm/tempfs.hpp"

namespace fs = std::filesystem;
using miu::shm::tempfs;

struct ut_buffer : public testing::Test {};

TEST_F(ut_buffer, default) {
    miu::shm::buffer buf;
    EXPECT_EQ(0U, buf.size());
    EXPECT_EQ(nullptr, buf.addr());
    EXPECT_FALSE(buf);
    EXPECT_TRUE(!buf);
}

TEST_F(ut_buffer, create) {
    miu::shm::buffer buf { "ut_buffer", 4096 };
    EXPECT_EQ(4096U, buf.size());

    EXPECT_TRUE(buf);
    EXPECT_TRUE(tempfs::exists("ut_buffer"));

    auto exp = fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read
               | fs::perms::group_write;
    auto status = fs::status(tempfs::join("ut_buffer"));
    EXPECT_EQ(exp, status.permissions());
}
