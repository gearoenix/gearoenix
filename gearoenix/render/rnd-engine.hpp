#ifndef GEAROENIX_RENDER_ENGINE_HPP
#define GEAROENIX_RENDER_ENGINE_HPP
#include "../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../vulkan/vk-engine.hpp"
#elif USE_DIRECT
#include "../directx12/dr-engine.hpp"
#else
#error "Not implemented"
#endif
#endif
