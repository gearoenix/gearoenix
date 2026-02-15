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
        {
            const std::lock_guard _lg(load_functions_lock);
            std::swap(load_functions, unload_functions);
        }
        for (const auto& f : unload_functions) {
            f();
        }
        unload_functions.clear();
    }
}

bool gearoenix::core::FunctionLoader::empty() const { return load_functions.empty(); }
