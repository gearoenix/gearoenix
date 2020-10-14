#ifndef GEAROENIX_CORE_FUNCTION_LOADER_HPP
#define GEAROENIX_CORE_FUNCTION_LOADER_HPP
#include "gx-cr-static.hpp"
#include <functional>
#include <vector>

namespace gearoenix::core {
class FunctionLoader {
private:
    GX_CREATE_GUARD(load_functions)
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