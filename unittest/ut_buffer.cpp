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
    EXPECT_EQ(0U, buf.size());
    EXPECT_EQ(nullptr, buf.addr());
    EXPECT_FALSE(buf);
    EXPECT_TRUE(!buf);
}

TEST_F(ut_buffer, create) {
    miu::shm::buffer buf { "ut_buffer", 4095 };
    EXPECT_EQ(4096U, buf.size());    // aliged to page size

    EXPECT_TRUE(buf);
    EXPECT_TRUE(tempfs::exists("ut_buffer"));
    EXPECT_EQ(buf.size(), tempfs::file_size("ut_buffer"));

    auto exp = fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read
               | fs::perms::group_write;
    auto status = fs::status(tempfs::join("ut_buffer"));
    EXPECT_EQ(exp, status.permissions());
}

TEST_F(ut_buffer, extend) {
    { miu::shm::buffer { "ut_buffer", 4096 }; }

    miu::shm::buffer buf { "ut_buffer", 8192 };
    EXPECT_EQ(8192U, buf.size());
}

TEST_F(ut_buffer, open) {
    { miu::shm::buffer { "ut_buffer", 4096 }; }

    miu::shm::buffer buf { "ut_buffer" };
    EXPECT_TRUE(buf);
    EXPECT_EQ(4096U, buf.size());
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
