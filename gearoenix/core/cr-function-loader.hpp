#ifndef GEAROENIX_CORE_FUNCTION_LOADER_HPP
#define GEAROENIX_CORE_FUNCTION_LOADER_HPP
#include "cr-build-configuration.hpp"
#include <functional>
#include <vector>

#ifndef GX_THREAD_NOT_SUPPORTED
#include <mutex>
#endif

namespace gearoenix::core {
class FunctionLoader {
private:
#ifndef GX_THREAD_NOT_SUPPORTED
    std::mutex locker;
#endif
    std::vector<std::function<void()>> load_functions;

public:
    FunctionLoader() noexcept;
    ~FunctionLoader() noexcept;
    void load(const std::function<void()>& fun) noexcept;
    void unload() noexcept;
    [[nodiscard]] unsigned int get_loaded_count() const noexcept;
};
}
#endif