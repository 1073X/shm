#include <fcntl.h>    // open
#include <gtest/gtest.h>

#include <meta/info.hpp>
#include <time/time.hpp>

#include "source/lib/buffer_impl.hpp"

using namespace std::chrono_literals;
using miu::shm::audit;
using miu::shm::buffer_impl;

TEST(ut_buffer_impl, init) {
    uint64_t buf[512] {};
    auto impl = new (buf) buffer_impl { "name", 4096, 512 };
    EXPECT_STREQ("name", impl->name());
    EXPECT_EQ(4096 - 512, impl->size());
    EXPECT_EQ(512, impl->offset());

    EXPECT_EQ(impl->begin(), impl->end());
}

TEST(ut_buffer_impl, audit_log) {
    miu::meta::set_category("ut_buffer_impl");
    miu::meta::set_type("ut_buffer_impl");
    miu::meta::set_name("ut_buffer_impl");

    uint64_t buf[512] {};
    auto impl = new (buf) buffer_impl { "name", 1024, 512 };

    auto fd  = ::open("ut_buffer_impl.audit_log", O_CREAT);
    auto now = miu::time::now();
    impl->add_audit(fd, "resize");
    impl->add_audit(fd, "open");
    ::close(fd);

    auto it = impl->begin();
    EXPECT_EQ(miu::meta::info(), it->info());
    EXPECT_EQ("resize", it->text());
    EXPECT_GT(1ms, it->time() - now);
    std::cout << miu::com::to_string(*it) << std::endl;

    it++;
    EXPECT_EQ("open", it->text());
    EXPECT_GT(1ms, it->time() - now);

    it++;
    EXPECT_EQ(impl->end(), it);
}

TEST(ut_buffer_impl, audit_wrapping) {
    uint64_t buf[512] {};
    auto impl = new (buf) buffer_impl { "name", 1024, 512 };

    auto fd = ::open("ut_buffer_impl.audit_wrapping", O_CREAT);

    auto max = (512 - sizeof(buffer_impl)) / sizeof(audit);
    for (auto i = 0U; i < max; i++) {
        impl->add_audit(fd, std::to_string(i));
    }

    impl->add_audit(fd, std::to_string(max));
    impl->add_audit(fd, std::to_string(max + 1));
    EXPECT_EQ(max + 2, impl->audit_size());

    ::close(fd);

    auto it = impl->begin();
    for (auto i = 2U; i < impl->audit_size(); i++) {
        EXPECT_EQ(std::to_string(i), (it++)->text());
    }
}
