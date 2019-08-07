#ifndef GEAROENIX_RENDER_GRAPH_TREE_PBR_HPP
#define GEAROENIX_RENDER_GRAPH_TREE_PBR_HPP
#include "../../../core/sync/cr-sync-end-caller.hpp"
#include "../../../core/cr-pool.hpp"
#include "rnd-gr-tr-tree.hpp"
#include <map>
#include <vector>
#include <tuple>
namespace gearoenix::render::graph::node {
    class ForwardPbr;
}
namespace gearoenix::render::light {
    class CascadeInfo;
}
namespace gearoenix::render::graph::tree {
class Pbr : public Tree {
private:
    bool in_weak_hardware = true;
    core::OneLoopPool<node::ForwardPbr> fwd;
    std::vector<std::vector<node::ForwardPbr*>> nodes;
    std::vector<light::CascadeInfo*> cascades;
public:
    Pbr(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    void update() noexcept final;
    void record(unsigned int kernel_index) noexcept final;
    void submit() noexcept final;
};
}
#endif