#include <gtest/gtest.h>

#include <fstream>

#include "shm/tempfs.hpp"

using miu::shm::tempfs;

TEST(ut_tempfs, path) { EXPECT_EQ("/dev/shm", tempfs.path()); }
