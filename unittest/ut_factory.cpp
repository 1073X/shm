#include <gtest/gtest.h>

#include "source/lib/factory.hpp"

using factory_type = miu::shm::factory<int32_t>;

static int32_t should_never_call() {
    throw "should never be called";
}

TEST(ut_factory, create) {
    // for the sake of code coverage
    try {
        should_never_call();
    } catch (const char*) {
    }

    factory_type factory;

    auto val = factory.create("item1", []() { return 1; });
    EXPECT_EQ(1, val);

    auto val2 = factory.create("item2", []() { return 2; });
    EXPECT_EQ(2, val2);

    // same name
    auto val3 = factory.create("item1", should_never_call);
    EXPECT_EQ(1, val3);
}

TEST(ut_factory, same) {
    factory_type factory;

    factory.create("item1", []() { return 1; });
    auto val2 = factory.create("item1", should_never_call);
    EXPECT_EQ(1, val2);
}

TEST(ut_factory, destory) {
    factory_type factory;

    factory.create("item1", []() { return 1; });
    factory.destory("item1");
    auto val2 = factory.create("item1", []() { return 2; });
    EXPECT_EQ(2, val2);
}

TEST(ut_factory, counter) {
    factory_type factory;

    factory.create("item1", []() { return 1; });
    factory.create("item1", should_never_call);
    factory.destory("item1");
    auto val = factory.create("item1", should_never_call);
    EXPECT_EQ(1, val);
}
