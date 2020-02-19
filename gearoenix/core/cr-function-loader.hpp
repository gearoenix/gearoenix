#ifndef GEAROENIX_CORE_FUNCTION_LOADER_HPP
#define GEAROENIX_CORE_FUNCTION_LOADER_HPP
#include <functional>
#include <mutex>
#include <vector>

namespace gearoenix::core {
class FunctionLoader {
private:
    std::mutex locker;
    std::vector<std::function<void()>> load_functions;

public:
    FunctionLoader() noexcept;
    ~FunctionLoader() noexcept;
    void load(std::function<void()> fun) noexcept;
    void unload() noexcept;
    [[nodiscard]] unsigned int get_loaded_count() const noexcept;
};
}
#endif