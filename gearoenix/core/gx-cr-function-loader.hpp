#ifndef GEAROENIX_CORE_FUNCTION_LOADER_HPP
#define GEAROENIX_CORE_FUNCTION_LOADER_HPP
#include <functional>
#include <mutex>
#include <vector>

namespace gearoenix::core {
struct FunctionLoader final {
private:
    std::mutex load_functions_lock;
    std::vector<std::function<void()>> load_functions;

public:
    FunctionLoader();
    ~FunctionLoader();
    void load(std::function<void()>&& fun);
    void unload();
    [[nodiscard]] std::size_t get_loaded_count() const;
};
}
#endif