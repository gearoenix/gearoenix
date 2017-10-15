#ifndef GEAROENIX_RENDER_PIPELINE_PIPELINE_HPP
#define GEAROENIX_RENDER_PIPELINE_PIPELINE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef USE_VULKAN
#include "../../vulkan/pipeline/vk-pip-pipeline.hpp"
#else
#error "Not implemented yet"
#endif
#endif // GEAROENIX_RENDER_PIPELINE_PIPELINE_HPP