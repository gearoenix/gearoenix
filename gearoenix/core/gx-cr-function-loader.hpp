#ifndef GEAROENIX_CORE_FUNCTION_LOADER_HPP
#define GEAROENIX_CORE_FUNCTION_LOADER_HPP
#include "../platform/macro/gx-plt-mcr-lock.hpp"
#include <functional>
#include <vector>

namespace gearoenix::core {
struct FunctionLoader final {
private:
    GX_CREATE_GUARD(load_functions);
    std::vector<std::function<void()>> load_functions;

public:
    FunctionLoader() noexcept;
    ~FunctionLoader() noexcept;
    void load(std::function<void()>&& fun) noexcept;
    void unload() noexcept;
    [[nodiscard]] std::size_t get_loaded_count() const noexcept;
};
}
#endif