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
    miu::shm::buffer buf { "ut_buffer.create", 4095 };
    EXPECT_EQ(4096U, buf.size());

    EXPECT_TRUE(buf);
    EXPECT_TRUE(tempfs::exists("ut_buffer.create"));
    EXPECT_EQ(buf.size(), tempfs::file_size("ut_buffer.create"));

    auto exp = fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read
               | fs::perms::group_write;
    auto status = fs::status(tempfs::join("ut_buffer.create"));
    EXPECT_EQ(exp, status.permissions());

    tempfs::remove("ut_buffer.create");
}

TEST_F(ut_buffer, extend) {
    { miu::shm::buffer { "ut_buffer.extend", 4096 }; }

    miu::shm::buffer buf { "ut_buffer.extend", 8192 };
    EXPECT_EQ(8192U, buf.size());

    tempfs::remove("ut_buffer.extend");
}

TEST_F(ut_buffer, open) {
    { miu::shm::buffer { "ut_buffer.open", 4096 }; }

    miu::shm::buffer buf { "ut_buffer.open" };
    EXPECT_TRUE(buf);
    EXPECT_EQ(4096U, buf.size());

    tempfs::remove("ut_buffer.open");
}

TEST_F(ut_buffer, open_failed) {
    miu::shm::buffer buf { "ut_buffer.open_failed" };
    EXPECT_FALSE(buf);
    tempfs::remove("ut_buffer.open_failed");
}
