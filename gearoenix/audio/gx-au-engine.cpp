#include "gx-au-engine.hpp"
#include <random>

gearoenix::audio::Engine::Engine(platform::Application&) noexcept
{
}

void gearoenix::audio::Engine::update() noexcept
{
    std::vector<std::uint16_t> samples(16);
    static std::default_random_engine random_engine;
    static std::uniform_int_distribution<std::uint16_t> noise_distribution;
    for (auto& s : samples)
        s = noise_distribution(random_engine);
    platform_engine.play(samples);
}
