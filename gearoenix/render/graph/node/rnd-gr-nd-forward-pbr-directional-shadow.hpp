#ifndef GEAROEMIX_RENDER_GRAPH_NODE_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#define GEAROEMIX_RENDER_GRAPH_NODE_FORWARD_PBR_DIRECTIONAL_SHADOW_HPP
#include "../../../core/sync/cr-sync-end-caller.hpp"
#include "../../../math/math-matrix.hpp"
#include "../../../math/math-vector.hpp"
#include "rnd-gr-nd-node.hpp"
#include <memory>
#include <vector>

namespace gearoenix {
namespace render {
    namespace buffer {
        class Uniform;
        class Manager;
    }
    namespace camera {
        class Camera;
    }
    namespace command {
        class Buffer;
    }
    namespace light {
        class Directional;
    }
    namespace model {
        class Model;
    }
    namespace pipeline {
        class Resource;
        class ResourceSet;
        class Pipeline;
        class ForwardPbrDirectionalShadow;
        class ForwardPbrDirectionalShadowResource;
        class ForwardPbrDirectionalShadowResourceSet;
    }
    namespace scene {
        class Scene;
    }
    namespace sync {
        class Semaphore;
    }
    namespace texture {
        class Cube;
        class Texture;
        class Texture2D;
    }
    namespace graph {
        namespace node {
            struct ForwardPbrDirectionalShadowKernel {
                std::shared_ptr<command::Buffer> secondary_cmd = nullptr;
                unsigned int latest_render_data_pool = 0;
                std::vector<
                    std::tuple<
                        std::shared_ptr<buffer::Uniform>,
                        std::shared_ptr<pipeline::ForwardPbrDirectionalShadowResourceSet>>>
                    render_data_pool;

                ForwardPbrDirectionalShadowKernel(const std::shared_ptr<engine::Engine> &e, const unsigned int kernel_index);
            };

            struct ForwardPbrDirectionalShadowFrame {
                std::shared_ptr<command::Buffer> primary_cmd = nullptr;
                std::shared_ptr<sync::Semaphore> semaphore = nullptr;
                std::shared_ptr<pipeline::ForwardPbrDirectionalShadowResource> pipeline_resource = nullptr;
                bool input_texture_changed = true;
                std::vector<ForwardPbrDirectionalShadowKernel> kernels;

                ForwardPbrDirectionalShadowFrame(const std::shared_ptr<engine::Engine> &e);
            };

            struct ForwardPbrDirectionalShadowUniform {
                math::Mat4x4 mvp = math::Mat4x4();
                math::Mat4x4 light_view_projection_biases;
                math::Vec4 light_color;
            };

            /// This renders only one directional light with one shadow map.
            /// It needs following textures:
            ///     0 - diffuse environment
            ///     1 - specular environment
            ///     2 - ambient occlusion
            ///     3 - shadow map
            ///     4 - brdflut
            /// In here I do not care for race issues (for performance reason).
            /// The user of this class must use its functionalities in their correct contextes.
            class ForwardPbrDirectionalShadow : public Node {
            private:
                std::vector<ForwardPbrDirectionalShadowFrame> frames;

            public:
                ForwardPbrDirectionalShadow(const std::shared_ptr<engine::Engine> &e, const core::sync::EndCaller<core::sync::EndCallerIgnore> &call);
                ~ForwardPbrDirectionalShadow();

                void set_diffuse_environment(const std::shared_ptr<texture::Cube>& t);
                void set_specular_environment(const std::shared_ptr<texture::Cube>& t);
                void set_ambient_occlusion(const std::shared_ptr<texture::Texture2D>& t);
                void set_shadow_mapper(const std::shared_ptr<texture::Texture2D>& t);
                void set_brdflut(const std::shared_ptr<texture::Texture2D>& t);

                void set_input_texture(const std::shared_ptr<texture::Texture>& t, const unsigned int index) override;
                /// This will be called at the start of each frame
                void update();
                void record(
                    const std::shared_ptr<scene::Scene>& s,
                    const std::shared_ptr<camera::Camera>& c,
                    const std::shared_ptr<light::Directional>& l,
                    const std::shared_ptr<model::Model>& m,
                    const unsigned int kernel_index);
                /// This will be called at the end of each frame for pushing jobs to GPU
                void submit();
            };
        }
    }
}
}
#endif