#ifndef GEAROENIX_RENDER_GRAPH_TREE_PBR_HPP
#define GEAROENIX_RENDER_GRAPH_TREE_PBR_HPP
#include "../../../core/cr-pool.hpp"
#include "../../../core/cr-types.hpp"
#include "../../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-gr-tr-tree.hpp"
#include <map>
#include <set>
#include <tuple>
#include <vector>

namespace gearoenix::render::graph::node {
class ForwardPbr;
class Unlit;
}
namespace gearoenix::render::light {
class CascadeInfo;
}
namespace gearoenix::render::scene {
class Scene;
}
namespace gearoenix::render::camera {
class Camera;
}
namespace gearoenix::render::graph::tree {
class Pbr : public Tree {
private:
    bool in_weak_hardware = true;
    core::OneLoopPool<node::ForwardPbr> fwdpbr;
    core::OneLoopPool<node::Unlit> unlit;
    std::map<core::Real, std::map<const scene::Scene*, std::map<core::Real, std::map<const camera::Camera*, node::ForwardPbr*>>>> nodes;
    std::vector<light::CascadeInfo*> cascades;

public:
    Pbr(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~Pbr() noexcept final;
    void update() noexcept final;
    void record(unsigned int kernel_index) noexcept final;
    void submit() noexcept final;
};
}
#endif