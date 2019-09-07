#ifndef GEAROENIX_RENDER_GRAPH_TREE_PBR_HPP
#define GEAROENIX_RENDER_GRAPH_TREE_PBR_HPP
#include "../../../core/sync/cr-sync-end-caller.hpp"
#include "../../../core/cr-pool.hpp"
#include "../../../core/cr-types.hpp"
#include "rnd-gr-tr-tree.hpp"
#include <map>
#include <vector>
#include <tuple>
#include <set>

namespace gearoenix::render::graph::node {
    class ForwardPbr;
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
    core::OneLoopPool<node::ForwardPbr> fwd;
    std::map<std::pair<core::Real, const scene::Scene*>, std::map<std::pair<core::Real, const camera::Camera *>, node::ForwardPbr*>> nodes;
    std::vector<light::CascadeInfo*> cascades;
public:
    Pbr(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    void update() noexcept final;
    void record(unsigned int kernel_index) noexcept final;
    void submit() noexcept final;
};
}
#endif