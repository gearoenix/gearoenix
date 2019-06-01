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
    FunctionLoader();
    ~FunctionLoader();
    void load(std::function<void()> fun);
    void unload();
    unsigned int get_loadeds_count() const;
};
}
#endif