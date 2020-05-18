#ifndef GEAROENIX_CORE_UPDATE_FUNCTIONS_MANAGER_HPP
#define GEAROENIX_CORE_UPDATE_FUNCTIONS_MANAGER_HPP
#include "cr-static.hpp"
#include "cr-types.hpp"
#include <functional>
#include <map>

namespace gearoenix::core::sync {
class KernelWorkers;
}

namespace gearoenix::core {
class UpdateFunctionsManager {
private:
    GX_CREATE_GUARD(update_functions)
    std::map<Id, std::function<void()>> update_functions;
    sync::KernelWorkers* const workers;

    void update_kernel(unsigned int kernel_index) noexcept;

public:
    explicit UpdateFunctionsManager(sync::KernelWorkers* workers) noexcept;
    ~UpdateFunctionsManager() noexcept;
    [[nodiscard]] Id add(std::function<void()> fun) noexcept;
    void remove(Id id) noexcept;
};
}
#endif