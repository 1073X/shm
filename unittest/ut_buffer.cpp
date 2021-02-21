#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <log/log.hpp>

#include "shm/buffer.hpp"
#include "shm/tempfs.hpp"
#include "source/lib/buffer_impl.hpp"

namespace fs = std::filesystem;
using namespace std::chrono_literals;
using miu::shm::tempfs;

struct ut_buffer : public testing::Test {
    void SetUp() override {
        using miu::log::severity;
        // miu::log::log::instance()->reset(severity::DEBUG, 1024);
    }
    void TearDown() override {
        tempfs::remove("ut_buffer");
        // miu::log::log::instance()->dump();
    }
};

TEST_F(ut_buffer, default) {
    miu::shm::buffer buf;
    EXPECT_FALSE(buf);
    EXPECT_TRUE(!buf);
}

TEST_F(ut_buffer, invalid) {
    miu::shm::buffer invalid { "0123456789abcdef", 4096 };
    EXPECT_FALSE(invalid);

    miu::shm::buffer empty { "", 4096 };
    EXPECT_FALSE(empty);
}

TEST_F(ut_buffer, make) {
    miu::shm::buffer buf { "ut_buffer", 4095 };
    EXPECT_EQ(4096U, buf.size());    // aliged to page size
    EXPECT_EQ("ut_buffer", buf.name());
    EXPECT_NE(nullptr, buf.data());

    EXPECT_TRUE(buf);
    EXPECT_TRUE(tempfs::exists("ut_buffer"));
    EXPECT_EQ(4096U * 2, tempfs::file_size("ut_buffer"));

    auto exp = fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read
             | fs::perms::group_write;
    auto status = fs::status(tempfs::join("ut_buffer"));
    EXPECT_EQ(exp, status.permissions());
}

TEST_F(ut_buffer, shrink) {
    { miu::shm::buffer buf { "ut_buffer", 8192 }; }

    miu::shm::buffer buf { "ut_buffer", 4096 };
    EXPECT_EQ(8192U, buf.size());
}

TEST_F(ut_buffer, extend) {
    { miu::shm::buffer buf { "ut_buffer", 4096 }; }

    miu::shm::buffer buf { "ut_buffer", 8192 };
    EXPECT_EQ(8192U, buf.size());
}

TEST_F(ut_buffer, open) {
    { miu::shm::buffer buf { "ut_buffer", 4096 }; }

    miu::shm::buffer buf { "ut_buffer" };
    EXPECT_TRUE(buf);
    EXPECT_EQ(4096U, buf.size());
    EXPECT_EQ("ut_buffer", buf.name());
}

TEST_F(ut_buffer, open_failed) {
    miu::shm::buffer buf { "ut_buffer" };
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

TEST_F(ut_buffer, resize) {
    miu::shm::buffer buf { "ut_buffer", 8192 };
    auto old_addr = buf.data();

    buf.resize(4096);
    EXPECT_EQ(8192U, buf.size());
    EXPECT_EQ(old_addr, buf.data());

    buf.resize(12288);
    EXPECT_EQ("ut_buffer", buf.name());
    EXPECT_EQ(12288U, buf.size());
}

TEST_F(ut_buffer, audit) {
    { miu::shm::buffer { "ut_buffer", 8192 }; }
    { miu::shm::buffer { "ut_buffer" }; }

    auto impl = miu::shm::buffer_impl::open("ut_buffer");
    EXPECT_EQ(5U, impl->audit_size());

    auto it = impl->begin();
    EXPECT_EQ("MAKE", (it++)->text());
    EXPECT_EQ("CLOSE", (it++)->text());
    EXPECT_EQ("OPEN", (it++)->text());
    EXPECT_EQ("CLOSE", (it++)->text());

    miu::shm::buffer_impl::close(impl);
}
