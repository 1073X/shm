#include <gtest/gtest.h>

#include "source/lib/buffer_impl.hpp"

using namespace std::chrono_literals;
using miu::shm::audit;
using miu::shm::buffer_impl;

TEST(ut_buffer_impl, init) {
    uint64_t buf[512];
    auto impl = new (buf) buffer_impl { "name", 4096, 128 };
    EXPECT_STREQ("name", impl->name());
    EXPECT_EQ(4096 - 128, impl->size());
    EXPECT_EQ(128, impl->offset());
}

TEST(ut_buffer_impl, audit_log) {
    miu::meta::set_category("ut_buffer_impl");
    miu::meta::set_type("ut_buffer_impl");
    miu::meta::set_name("ut_buffer_impl");

    uint64_t buf[512] {};
    auto impl = new (buf) buffer_impl { "name", 1024, 512 };

    auto now = miu::com::datetime::now();
    impl->add_audit_log("resize");
    impl->add_audit_log("open");

    auto audits = (audit const*)((const char*)buf + sizeof(buffer_impl));

    EXPECT_EQ("ut_buffer_impl", audits[0].category());
    EXPECT_EQ("ut_buffer_impl", audits[0].type());
    EXPECT_EQ("ut_buffer_impl", audits[0].name());
    EXPECT_EQ("resize", audits[0].text());
    EXPECT_GT(1ms, audits[0].time() - now);

    EXPECT_EQ("ut_buffer_impl", audits[1].category());
    EXPECT_EQ("ut_buffer_impl", audits[1].type());
    EXPECT_EQ("ut_buffer_impl", audits[1].name());
    EXPECT_EQ("open", audits[1].text());
    EXPECT_GT(1ms, audits[1].time() - audits[0].time());
}
