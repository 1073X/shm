#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <log/log.hpp>

#include "shm/buffer.hpp"
#include "shm/tempfs.hpp"
#include "source/lib/buffer_impl.hpp"

namespace fs = std::filesystem;
using namespace std::chrono_literals;
using miu::shm::buffer;
using miu::shm::mode;
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
    buffer buf;
    EXPECT_FALSE(buf);
    EXPECT_TRUE(!buf);
    EXPECT_EQ(0U, buf.size());
    EXPECT_EQ(mode::MAX, buf.mode());
}

TEST_F(ut_buffer, invalid) {
    buffer invalid { "0123456789abcdef", 4096 };
    EXPECT_FALSE(invalid);
    EXPECT_EQ(0U, invalid.size());
    EXPECT_EQ(mode::MAX, invalid.mode());

    buffer empty { "", 4096 };
    EXPECT_FALSE(empty);
    EXPECT_EQ(0U, empty.size());
    EXPECT_EQ(mode::MAX, empty.mode());
}

TEST_F(ut_buffer, make) {
    buffer buf { "ut_buffer", 4095 };
    EXPECT_EQ(4096U, buf.size());    // aliged to page size
    EXPECT_EQ("ut_buffer", buf.name());
    EXPECT_NE(nullptr, buf.data());
    EXPECT_EQ(buf.data(), const_cast<buffer const&>(buf).data());
    EXPECT_EQ(mode::RDWR, buf.mode());

    EXPECT_TRUE(buf);
    EXPECT_TRUE(tempfs::exists("ut_buffer"));
    EXPECT_EQ(4096U * 2, tempfs::file_size("ut_buffer"));

    auto exp = fs::perms::owner_read | fs::perms::owner_write | fs::perms::group_read
             | fs::perms::group_write;
    auto status = fs::status(tempfs::join("ut_buffer"));
    EXPECT_EQ(exp, status.permissions());
}

TEST_F(ut_buffer, shrink) {
    { buffer buf { "ut_buffer", 8192 }; }

    buffer buf { "ut_buffer", 4096 };
    EXPECT_EQ(8192U, buf.size());
}

TEST_F(ut_buffer, extend) {
    { buffer buf { "ut_buffer", 4096 }; }

    buffer buf { "ut_buffer", 8192 };
    EXPECT_EQ(8192U, buf.size());
}

TEST_F(ut_buffer, open_read) {
    { buffer buf { "ut_buffer", 4096 }; }

    buffer buf { "ut_buffer", mode::READ };
    EXPECT_TRUE(buf);
    EXPECT_EQ(4096U, buf.size());
    EXPECT_EQ("ut_buffer", buf.name());
    EXPECT_EQ(mode::READ, buf.mode());
}

TEST_F(ut_buffer, open_rdwr) {
    { buffer buf { "ut_buffer", 4096 }; }

    buffer buf { "ut_buffer", mode::RDWR };
    EXPECT_TRUE(buf);
    EXPECT_EQ(4096U, buf.size());
    EXPECT_EQ("ut_buffer", buf.name());
    EXPECT_EQ(mode::RDWR, buf.mode());
}

TEST_F(ut_buffer, open_failed) {
    EXPECT_FALSE(buffer("ut_buffer", mode::READ));
    EXPECT_FALSE(buffer("ut_buffer", mode::RDWR));
}

TEST_F(ut_buffer, open_0) {
    std::ofstream ss { tempfs::join("ut_buffer") };
    ss << "ut_buffer";
    ss.close();
    EXPECT_TRUE(tempfs::exists("ut_buffer"));

    EXPECT_FALSE(buffer("ut_buffer", mode::READ));
    EXPECT_FALSE(buffer("ut_buffer", mode::RDWR));
}

TEST_F(ut_buffer, duplicated) {
    {    // create -> open
        buffer buf { "ut_buffer", 4096 };
        EXPECT_TRUE(buf);
        EXPECT_FALSE(buffer("ut_buffer", mode::RDWR));
    }
    {    // open -> create
        buffer buf { "ut_buffer", mode::RDWR };
        EXPECT_TRUE(buf);
        EXPECT_FALSE(buffer("ut_buffer", 4096));
    }
}

TEST_F(ut_buffer, move) {
    buffer buf1 { "ut_buffer", 4096 };

    buffer buf2 { std::move(buf1) };
    EXPECT_TRUE(buf2);
    EXPECT_EQ(4096U, buf2.size());
    EXPECT_EQ(mode::RDWR, buf2.mode());

    EXPECT_FALSE(buf1);    // NOLINT: testing move
    EXPECT_EQ(0U, buf1.size());
    EXPECT_EQ(mode::MAX, buf1.mode());

    buffer buf3;
    buf3 = std::move(buf2);
    EXPECT_TRUE(buf3);
    EXPECT_EQ(4096U, buf3.size());
    EXPECT_EQ(mode::RDWR, buf3.mode());

    EXPECT_FALSE(buf2);    // NOLINT: testing move
    EXPECT_EQ(0U, buf2.size());
    EXPECT_EQ(mode::MAX, buf2.mode());
}

TEST_F(ut_buffer, resize) {
    buffer buf { "ut_buffer", 8192 };
    auto old_addr = buf.data();

    buf.resize(4096);
    EXPECT_EQ(8192U, buf.size());
    EXPECT_EQ(old_addr, buf.data());

    buf.resize(12288);
    EXPECT_EQ("ut_buffer", buf.name());
    EXPECT_EQ(12288U, buf.size());
}

TEST_F(ut_buffer, resize_rdwr) {
    buffer { "ut_buffer", 8192 };    // create

    // open read-write and resize
    buffer buf { "ut_buffer", mode::RDWR };
    buf.resize(12288);
    EXPECT_EQ(12288U, buf.size());
}

TEST_F(ut_buffer, resize_read) {
    buffer { "ut_buffer", 8192 };    // create

    // cann't reisze read-only
    buffer buf { "ut_buffer", mode::READ };
    EXPECT_FALSE(buf.resize(12288));
    EXPECT_EQ(8192U, buf.size());
}

TEST_F(ut_buffer, audit) {
    { buffer { "ut_buffer", 8192 }; }
    { buffer { "ut_buffer", mode::READ }; }    // no audit for read only
    { buffer { "ut_buffer", mode::RDWR }; }

    auto impl = miu::shm::buffer_impl::open("ut_buffer", mode::RDWR);
    EXPECT_EQ(5U, impl->audit_size());

    auto it = impl->begin();
    EXPECT_EQ("MAKE", (it++)->text());
    EXPECT_EQ("CLOSE", (it++)->text());
    EXPECT_EQ("OPEN", (it++)->text());
    EXPECT_EQ("CLOSE", (it++)->text());

    miu::shm::buffer_impl::close(impl, mode::RDWR);
}
