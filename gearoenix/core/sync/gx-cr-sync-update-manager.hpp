#ifndef GEAROENIX_CORE_SYNC_UPDATE_MANAGER_HPP
#define GEAROENIX_CORE_SYNC_UPDATE_MANAGER_HPP
#include "gx-cr-sync-influence-manager.hpp"

namespace gearoenix::core::sync {
struct KernelWorkers;
struct UpdateManager {
private:
    InfluenceManager influence_manager;
    sync::KernelWorkers* const workers;

    void update();
    void update_kernel(unsigned int kernel_index);

public:
    explicit UpdateManager(sync::KernelWorkers* workers);
    ~UpdateManager();
    [[nodiscard]] Id add(std::vector<Id> influenced_ids, double priority, std::function<void()> fun);
    void remove(Id id);
};
}
#endif