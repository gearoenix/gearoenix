#ifndef GEAROENIX_RENDER_GRAPH_TREE_TREE_HPP
#define GEAROENIX_RENDER_GRAPH_TREE_TREE_HPP
#include "../../../core/cr-static.hpp"

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::graph::tree {
enum struct RuntimeReflectionState {
    /// This a long processing state but because of integrity of the cube map in highly dynamic scenes,
    /// it can not be broken into faces.
    EnvironmentCubeRender = 0,
    EnvironmentCubeMipMap = 1,
    IrradianceFace = 2,
    IrradianceMipMap = 3,
    RadianceFaceLevel = 4,
    Resting = 7,
};
class Tree {
    GX_GETSET_VAL_PRT(RuntimeReflectionState, runtime_reflection_state, RuntimeReflectionState::Resting)
    GX_GETSET_VAL_PRT(std::size_t, runtime_reflections_irradiance_face, 0)
    GX_GETSET_VAL_PRT(std::size_t, runtime_reflections_radiance_face, 0)
    GX_GETSET_VAL_PRT(std::size_t, runtime_reflections_radiance_level, 0)
protected:
    engine::Engine* const e;
    explicit Tree(engine::Engine* const e) noexcept
        : e(e)
    {
    }

public:
    virtual ~Tree() noexcept = default;
    /// It is called from graphic thread
    virtual void update() noexcept = 0;
    /// Multithreaded rendering happens in here
    virtual void record(unsigned int kernel_index) noexcept = 0;
    /// Submit is called from graphic thread
    virtual void submit() noexcept = 0;
};
}

#endif