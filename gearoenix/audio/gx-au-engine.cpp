#include "gx-au-engine.hpp"
#include "../core/sync/gx-cr-sync-parallel-for.hpp"
#include "gx-au-constants.hpp"
#include <random>

void gearoenix::audio::Engine::compositor_loop() noexcept
{
    compositor_thread_terminated = false;
    while (!compositor_thread_terminated) {
        // compositor_signal.lock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (compositor_thread_terminated)
            break;
        composit();
    }
    compositor_thread_not_joined = true;
}

void gearoenix::audio::Engine::composit() noexcept
{
    constexpr auto needed_sample_coef = static_cast<double>(sampler_rate);

    const auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> delta_time_points = current_time - last_composition_time;
    last_composition_time = current_time;
    const double delta_time = delta_time_points.count();
    const auto needed_samples = static_cast<int>(needed_sample_coef * delta_time);
    const auto needed_samples_to_process = needed_samples; // -platform_engine.in_queue_samples_count();
    if (needed_samples_to_process <= 0)
        return;
    composited_samples.resize(needed_samples_to_process);
    /*for (auto& cs : threads_composited_samples) {
        cs.resize(needed_samples_to_process);
        std::memset(cs.data(), 0, needed_samples_to_process * sizeof(std::uint16_t));
    }
    {
        std::lock_guard<std::mutex> _lg(manager.this_lock);
        core::sync::ParallelFor::exec(
            manager.players.begin(),
            manager.players.end(),
            [&](Player& player, const unsigned int kernel_index) noexcept {
                manager.fill_samples(player, threads_composited_samples[kernel_index]);
            });
    }
    core::sync::ParallelFor::execi(
        composited_samples.begin(),
        composited_samples.end(),
        [&](std::uint16_t& sample, const unsigned int index, const unsigned int) noexcept {
            sample = 0;
            for (const auto& cs : threads_composited_samples) {
                sample += cs[index];
            }
        });*/

    if (!manager.players.empty())
        manager.fill_samples(manager.players[0], composited_samples);
    platform_engine.play(composited_samples);

    // static int iii = 0;
    // if (!manager.audios.empty() && iii == 0)
    //{
    //     ++iii;
    //     platform_engine.play(manager.audios[0].samples);
    //     if (iii > 1000000) iii = 0;
    // }
}

gearoenix::audio::Engine::Engine(platform::Application& app) noexcept
    : platform_application(app)
    , manager(app)
    , last_composition_time(std::chrono::high_resolution_clock::now())
    , threads_composited_samples(std::thread::hardware_concurrency())
    , compositor_thread_terminated(false)
    , compositor_thread_not_joined(true)
    , compositor_thread([this] { compositor_loop(); })
{
}

gearoenix::audio::Engine::~Engine() noexcept
{
    for (compositor_thread_terminated = true; compositor_thread_not_joined; compositor_signal.release())
        ;
    compositor_thread.join();
}

void gearoenix::audio::Engine::update() noexcept
{
    // compositor_signal.release();
    // composit();
}
