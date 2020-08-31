#ifndef GEAROENIX_CORE_SYNC_UPDATE_MANAGER_HPP
#define GEAROENIX_CORE_SYNC_UPDATE_MANAGER_HPP
#include "gx-cr-sync-influence-manager.hpp"

namespace gearoenix::core::sync {
class KernelWorkers;
class UpdateManager {
private:
    InfluenceManager influence_manager;
    sync::KernelWorkers* const workers;

    void update() noexcept;
    void update_kernel(unsigned int kernel_index) noexcept;

public:
    explicit UpdateManager(sync::KernelWorkers* workers) noexcept;
    ~UpdateManager() noexcept;
    [[nodiscard]] Id add(std::vector<Id> influenced_ids, double priority, std::function<void()> fun) noexcept;
    void remove(Id id) noexcept;
};
}
#endif