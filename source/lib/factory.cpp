
#include "factory.hpp"

namespace miu::shm {

class buffer_impl;

factory<buffer_impl*> g_impl_factory;

}    // namespace miu::shm
