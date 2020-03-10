#include "rnd-gr-tr-tree.hpp"
#include "../../../core/cr-build-configuration.hpp"

void gearoenix::render::graph::tree::Tree::update_runtime_reflection_state() noexcept
{
    switch (runtime_reflection_state) {
    case RuntimeReflectionState::EnvironmentCubeRender:
        runtime_reflection_state = RuntimeReflectionState::EnvironmentCubeMipMap;
        return;
    case RuntimeReflectionState::EnvironmentCubeMipMap:
        runtime_reflection_state = RuntimeReflectionState::IrradianceFace;
        runtime_reflections_irradiance_face = 0;
        return;
    case RuntimeReflectionState::IrradianceFace:
        ++runtime_reflections_irradiance_face;
        if (runtime_reflections_irradiance_face > 5) {
            runtime_reflection_state = RuntimeReflectionState::IrradianceMipMap;
        }
        return;
    case RuntimeReflectionState::IrradianceMipMap:
        runtime_reflection_state = RuntimeReflectionState::RadianceFaceLevel;
        runtime_reflections_radiance_face = 0;
        return;
    case RuntimeReflectionState::RadianceFaceLevel:
        ++runtime_reflections_radiance_level;
        if (runtime_reflections_radiance_level >= GX_MAX_RUNTIME_REFLECTION_RADIANCE_LEVELS) {
            runtime_reflections_radiance_level = 0;
            ++runtime_reflections_radiance_face;
            if (runtime_reflections_radiance_face > 5) {
                runtime_reflection_state = RuntimeReflectionState::EnvironmentCubeRender;
            }
        }
        return;
    case RuntimeReflectionState::Resting:
        runtime_reflection_state = RuntimeReflectionState::EnvironmentCubeRender;
        return;
    }
}