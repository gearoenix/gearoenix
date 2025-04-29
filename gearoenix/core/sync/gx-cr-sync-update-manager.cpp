// #include "gx-cr-sync-update-manager.hpp"
// #include "gx-cr-sync-kernel-workers.hpp"
//
// void gearoenix::core::sync::UpdateManager::update()
// {
//     influence_manager.update();
// }
//
// void gearoenix::core::sync::UpdateManager::update_kernel(const unsigned int kernel_index)
// {
//     GX_START_MULTITHREADED_TASKS
//     const unsigned int kernels_count = workers->get_threads_count();
//     std::uint32_t current_index = 0;
//     const auto& end_indices = influence_manager.get_influencers_indices();
//     const auto& functions = influence_manager.get_influencers();
//     for (const std::uint32_t end_index : end_indices) {
//         GX_DO_MULTITHREADED_TASK(for (; current_index < end_index; ++current_index) {
//             functions[current_index]();
//         })
//     }
// }
//
// gearoenix::core::sync::UpdateManager::UpdateManager(sync::KernelWorkers* const workers)
//     : workers(workers)
// {
//     workers->add_step(
//         [this] { update(); },
//         [this](const unsigned int ki) { update_kernel(ki); },
//         [] {},
//         [] {});
// }
//
// gearoenix::core::sync::UpdateManager::~UpdateManager() = default;
//
// gearoenix::core::Id gearoenix::core::sync::UpdateManager::add(
//     std::vector<Id> influenced_ids,
//     const double priority,
//     std::function<void()> fun)
// {
//     return influence_manager.add(std::move(influenced_ids), priority, std::move(fun));
// }
//
// void gearoenix::core::sync::UpdateManager::remove(const Id id)
// {
//     influence_manager.remove(id);
// }
