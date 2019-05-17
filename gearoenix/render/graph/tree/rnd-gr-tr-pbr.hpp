#ifndef GEAROENIX_RENDER_GRAPH_TREE_PBR_HPP
#define GEAROENIX_RENDER_GRAPH_TREE_PBR_HPP
#include "../../../core/sync/cr-sync-end-caller.hpp"
#include "rnd-gr-tr-tree.hpp"
namespace gearoenix {
namespace render {
    namespace graph {
        namespace node {
            class ForwardPbrDirectionalShadow;
            class ShadowMapper;
        }
        namespace tree {
            class Pbr : public Tree {
            private:
                const bool in_weak_hardware;
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
                Pbr(
                    const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call);
                ~Pbr() override final;
                void update() override final;
                void record(const unsigned int kernel_index) override final;
                void submit() override final;
            };
        }
    }
}
}
#endif