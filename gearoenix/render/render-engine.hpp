#ifndef GEAROENIX_NUFRAG_RENDER_ENGINE_HPP
#define GEAROENIX_NUFRAG_RENDER_ENGINE_HPP
#include "../core/build-configuration.hpp"
#ifdef USE_VULKAN
#include "../vulkan/vk-engine.hpp"
#elif USE_DIRECT
#include "../directx12/dr-engine.hpp"
#else
#error "Not implemented"
#endif
#endif
