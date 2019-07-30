#ifndef GEAROENIX_RENDER_GRAPH_TREE_PBR_HPP
#define GEAROENIX_RENDER_GRAPH_TREE_PBR_HPP
#include "../../../core/sync/cr-sync-end-caller.hpp"
#include "../../../core/cr-pool.hpp"
#include "rnd-gr-tr-tree.hpp"
namespace gearoenix::render::graph {
namespace node {
    class ForwardPbr;
}
namespace tree {
    class Pbr : public Tree {
    private:
        bool in_weak_hardware = true;
        core::OneLoopPool<node::ForwardPbr> fwd;
        // std::shared_ptr<node::DeferedPbr> dfr = nullptr;
        // std::shared_ptr<node::SahdowAccumulator> shdacm = nullptr;
    public:
        Pbr(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
        void update() noexcept final;
        void record(unsigned int kernel_index) noexcept final;
        void submit() noexcept final;
    };
}
}
#endif