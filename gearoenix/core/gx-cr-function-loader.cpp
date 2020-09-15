#include "gx-cr-function-loader.hpp"
#include "../system/gx-sys-log.hpp"

gearoenix::core::FunctionLoader::FunctionLoader() noexcept = default;

gearoenix::core::FunctionLoader::~FunctionLoader() noexcept
{
    if (load_functions.empty())
        return;
    GXLOGE("Function loader queue was not empty when the deconstructor called.")
}

void gearoenix::core::FunctionLoader::load(const std::function<void()>& fun) noexcept
{
    GX_GUARD_LOCK(load_functions)
    load_functions.push_back(fun);
}

void gearoenix::core::FunctionLoader::unload() noexcept
{
    while (!load_functions.empty()) {
        std::vector<std::function<void()>> functions;
        {
            GX_GUARD_LOCK(load_functions)
            std::swap(load_functions, functions);
        }
        for (const auto& f : functions) {
            f();
        }
    }
}

unsigned int gearoenix::core::FunctionLoader::get_loaded_count() const noexcept
{
    return static_cast<unsigned int>(load_functions.size());
}
