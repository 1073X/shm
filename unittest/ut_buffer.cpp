#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "shm/buffer.hpp"
#include "shm/tempfs.hpp"

namespace fs = std::filesystem;
using miu::shm::tempfs;

struct ut_buffer : public testing::Test {
    void TearDown() override { tempfs::remove("ut_buffer"); };
};

TEST_F(ut_buffer, default) {
    miu::shm::buffer buf;
    EXPECT_FALSE(buf);
    EXPECT_TRUE(!buf);
}

TEST_F(ut_buffer, create) {
    miu::shm::buffer buf { "ut_buffer", 4095 };
    EXPECT_EQ(4096U - CACHE_LINE, buf.size());    // aliged to page size
    EXPECT_STREQ("ut_buffer", buf.name());
    EXPECT_EQ(buf.name() + CACHE_LINE, buf.addr());

    EXPECT_TRUE(buf);
    EXPECT_TRUE(tempfs::exists("ut_buffer"));
    EXPECT_EQ(4096U, tempfs::file_size("ut_buffer"));

    auto exp = fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read
             | fs::perms::group_write;
    auto status = fs::status(tempfs::join("ut_buffer"));
    EXPECT_EQ(exp, status.permissions());
}

TEST_F(ut_buffer, extend) {
    { miu::shm::buffer { "ut_buffer", 4096 }; }

    miu::shm::buffer buf { "ut_buffer", 8192 };
    EXPECT_EQ(8192U - CACHE_LINE, buf.size());
}

TEST_F(ut_buffer, open) {
    { miu::shm::buffer { "ut_buffer", 4096 }; }

    miu::shm::buffer buf { "ut_buffer" };
    EXPECT_TRUE(buf);
    EXPECT_EQ(4096U - CACHE_LINE, buf.size());
    EXPECT_STREQ("ut_buffer", buf.name());
}

TEST_F(ut_buffer, open_failed) {
    miu::shm::buffer buf { "ut_buffer" };
    EXPECT_FALSE(buf);
}

TEST_F(ut_buffer, create_0) {
    miu::shm::buffer buf { "ut_buffer", 0 };
    EXPECT_FALSE(buf);
}

TEST_F(ut_buffer, open_0) {
    std::ofstream ss { tempfs::join("ut_buffer") };
    ss << "ut_buffer";
    ss.close();
    EXPECT_TRUE(tempfs::exists("ut_buffer"));

    miu::shm::buffer buf { "ut_buffer" };
    EXPECT_FALSE(buf);
}

TEST_F(ut_buffer, duplicated) {
    {    // create -> open
        miu::shm::buffer buf { "ut_buffer", 4096 };
        EXPECT_TRUE(buf);
        EXPECT_FALSE(miu::shm::buffer("ut_buffer"));
    }
    {    // open -> create
        miu::shm::buffer buf { "ut_buffer" };
        EXPECT_TRUE(buf);
        EXPECT_FALSE(miu::shm::buffer("ut_buffer", 4096));
    }
}

TEST_F(ut_buffer, move) {
    miu::shm::buffer buf1 { "ut_buffer", 4096 };

    miu::shm::buffer buf2 { std::move(buf1) };
    EXPECT_TRUE(buf2);
    EXPECT_FALSE(buf1);    // NOLINT: testing move

    miu::shm::buffer buf3;
    buf3 = std::move(buf2);
    EXPECT_TRUE(buf3);
    EXPECT_FALSE(buf2);    // NOLINT: testing move
}
