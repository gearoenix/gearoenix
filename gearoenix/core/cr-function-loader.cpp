#include "cr-function-loader.hpp"
#include "../system/sys-log.hpp"

gearoenix::core::FunctionLoader::FunctionLoader() noexcept = default;

gearoenix::core::FunctionLoader::~FunctionLoader() noexcept
{
    if (load_functions.empty())
        return;
    GXLOGE("Function loader queue was not empty when the deconstructor called.")
}

void gearoenix::core::FunctionLoader::load(const std::function<void()>& fun) noexcept
{
#ifndef GX_THREAD_NOT_SUPPORTED
    std::lock_guard<std::mutex> _l(locker);
#endif
    load_functions.push_back(fun);
}

void gearoenix::core::FunctionLoader::unload() noexcept
{
    while (!load_functions.empty()) {
        std::vector<std::function<void()>> funs;
        {
#ifndef GX_THREAD_NOT_SUPPORTED
            std::lock_guard<std::mutex> _l(locker);
#endif
            std::swap(load_functions, funs);
        }
        for (const auto& f : funs) {
            f();
        }
    }
}

unsigned int gearoenix::core::FunctionLoader::get_loaded_count() const noexcept
{
    return static_cast<unsigned int>(load_functions.size());
}
