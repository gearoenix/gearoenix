#ifndef GEAROENIX_RENDER_GRAPH_TREE_PBR_HPP
#define GEAROENIX_RENDER_GRAPH_TREE_PBR_HPP
#include "../../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-gr-tr-tree.hpp"
namespace gearoenix::render::graph {
namespace node {
    class ForwardPbrDirectionalShadow;
    class ShadowMapper;
}
namespace tree {
    class Pbr : public Tree {
    private:
        bool in_weak_hardware = true;
        // std::shared_ptr<node::ForwardPbrConeShadow> fwdconshd = nullptr;
        std::shared_ptr<node::ForwardPbrDirectionalShadow> fwddirshd = nullptr;
        // std::shared_ptr<node::ForwardPbrPointShadow> fwdpointshd = nullptr;
        // std::shared_ptr<node::DeferedPbr> deferred = nullptr;
        // std::shared_ptr<node::SahdowAccumulatorCone> shdacmcone = nullptr;
        // std::shared_ptr<node::SahdowAccumulatorDirectional> shdacmdir = nullptr;
        // std::shared_ptr<node::SahdowAccumulatorPoint> shdacmpnt = nullptr;
        /// TODO: In future, this should become several for each light frustum
        //std::shared_ptr<node::ShadowMapper> shdmap = nullptr;
    public:
        Pbr(engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
        void update() noexcept final;
        void record(unsigned int kernel_index) noexcept final;
        void submit() noexcept final;
    };
}
}
#endif