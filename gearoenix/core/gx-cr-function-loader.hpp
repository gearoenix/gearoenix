#pragma once
#include <functional>
#include <mutex>
#include <vector>

namespace gearoenix::core {
struct FunctionLoader final {
private:
    std::mutex load_functions_lock;
    std::vector<std::function<void()>> load_functions;
    std::vector<std::function<void()>> unload_functions;

public:
    FunctionLoader();
    ~FunctionLoader();
    void load(std::function<void()>&& fun);
    void unload();
    [[nodiscard]] bool empty() const;
};
}