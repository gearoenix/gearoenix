#include "gx-cr-function-loader.hpp"
#include "../platform/gx-plt-log.hpp"

gearoenix::core::FunctionLoader::FunctionLoader() noexcept = default;

gearoenix::core::FunctionLoader::~FunctionLoader() noexcept
{
    if (load_functions.empty())
        return;
    GX_LOG_E("Function loader queue was not empty when deconstructed.");
}

void gearoenix::core::FunctionLoader::load(std::function<void()>&& fun) noexcept
{
    std::lock_guard<std::mutex> _lg(load_functions_lock);
    load_functions.push_back(std::move(fun));
}

void gearoenix::core::FunctionLoader::unload() noexcept
{
    while (!load_functions.empty()) {
        std::vector<std::function<void()>> functions;
        {
            std::lock_guard<std::mutex> _lg(load_functions_lock);
            std::swap(load_functions, functions);
        }
        for (const auto& f : functions) {
            f();
        }
    }
}

std::size_t gearoenix::core::FunctionLoader::get_loaded_count() const noexcept
{
    return load_functions.size();
}
