
#include <cfg/cmd_source.hpp>
#include <cfg/settings.hpp>
#include <com/fatal_error.hpp>
#include <iomanip>
#include <meta/info.hpp>

#include "shm/version.hpp"
#include "source/lib/buffer_impl.hpp"

int32_t main(int32_t argc, const char* argv[]) try {
    miu::cfg::cmd_source source { argc, argv };
    miu::cfg::settings settings { &source };

    if (settings.optional<bool>("version", false)) {
        std::cout << miu::meta::info() << ": dump shm buffer metainfo and audit logs." << std::endl;
        std::cout << miu::shm::version() << std::endl;
        std::cout << miu::shm::build_info() << std::endl;
        std::cout << "\nUsage: shmview [shm]" << std::endl;
    } else {
        auto name = settings.required<std::string>(0);
        auto impl = miu::shm::buffer_impl::open(name);
        if (!impl) {
            FATAL_ERROR<std::runtime_error>("cannot find shm", name);
        }

        std::cout << impl->name() << " CAP: " << impl->size() << "(bytes)" << std::endl;
        for (auto const& audit : *impl) {
            std::cout << miu::com::to_string(audit) << std::endl;
        }

        miu::shm::buffer_impl::close(impl);
    }

    return 0;
} catch (std::exception const& err) {
    std::cerr << err.what() << std::endl;
    return -1;
}
