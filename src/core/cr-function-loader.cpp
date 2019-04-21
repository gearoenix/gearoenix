#include "cr-function-loader.hpp"

gearoenix::core::FunctionLoader::FunctionLoader()
{
}

gearoenix::core::FunctionLoader::~FunctionLoader()
{
    std::lock_guard<std::mutex> _l(locker);
    load_functions.clear();
}

void gearoenix::core::FunctionLoader::load(std::function<void()> fun)
{
    std::lock_guard<std::mutex> _l(locker);
    load_functions.push_back(fun);
}

void gearoenix::core::FunctionLoader::unload()
{
    std::vector<std::function<void()>> funs;
    {
        std::lock_guard<std::mutex> _l(locker);
        std::swap(load_functions, funs);
    }
    for (const auto& f : funs) {
        f();
    }
}

unsigned int gearoenix::core::FunctionLoader::get_loadeds_count() const
{
    return static_cast<unsigned int>(load_functions.size());
}
