#include "gx-cr-function-loader.hpp"
#include "../platform/gx-plt-log.hpp"

gearoenix::core::FunctionLoader::FunctionLoader() = default;

gearoenix::core::FunctionLoader::~FunctionLoader()
{
    if (load_functions.empty()) {
        return;
    }
    GX_LOG_E("Function loader queue was not empty when deconstructed.");
}

void gearoenix::core::FunctionLoader::load(std::function<void()>&& fun)
{
    const std::lock_guard _lg(load_functions_lock);
    load_functions.push_back(std::move(fun));
}

void gearoenix::core::FunctionLoader::unload()
{
    while (!load_functions.empty()) {
        std::vector<std::function<void()>> functions;
        {
            const std::lock_guard _lg(load_functions_lock);
            std::swap(load_functions, functions);
        }
        for (const auto& f : functions) {
            f();
        }
    }
}

std::uint64_t gearoenix::core::FunctionLoader::get_loaded_count() const
{
    return static_cast<std::uint64_t>(load_functions.size());
}
