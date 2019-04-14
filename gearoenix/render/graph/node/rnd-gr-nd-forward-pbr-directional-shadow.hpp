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
        class Mesh;
    }
    namespace pipeline {
        class ForwardPbrDirectionalShadow;
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
                std::vector<std::shared_ptr<pipeline::ForwardPbrDirectionalShadowResourceSet>> render_data_pool;
                ForwardPbrDirectionalShadowKernel(const std::shared_ptr<engine::Engine>& e, const unsigned int kernel_index);
            };

            struct ForwardPbrDirectionalShadowFrame {
                std::shared_ptr<command::Buffer> primary_cmd = nullptr;
                std::shared_ptr<sync::Semaphore> semaphore = nullptr;
                std::vector<std::shared_ptr<ForwardPbrDirectionalShadowKernel>> kernels;
                ForwardPbrDirectionalShadowFrame(const std::shared_ptr<engine::Engine>& e);
            };

            /// This renders only one directional light with one shadow map.
            /// In here I do not care for race issues (for performance reason).
            /// The user of this class must use its functionalities in their correct contextes.
            class ForwardPbrDirectionalShadow : public Node {
            private:
                std::vector<std::shared_ptr<ForwardPbrDirectionalShadowFrame>> frames;

            public:
                const static unsigned int diffuse_environment_index;
                const static unsigned int specular_environment_index;
                const static unsigned int ambient_occlusion_index;
                const static unsigned int shadow_map_index;
                const static unsigned int brdflut_index;

                ForwardPbrDirectionalShadow(const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call);
                ~ForwardPbrDirectionalShadow() override final;

                void set_diffuse_environment(const std::shared_ptr<texture::Cube>& t);
                void set_specular_environment(const std::shared_ptr<texture::Cube>& t);
                void set_ambient_occlusion(const std::shared_ptr<texture::Texture2D>& t);
                void set_shadow_mapper(const std::shared_ptr<texture::Texture2D>& t);
                void set_brdflut(const std::shared_ptr<texture::Texture2D>& t);

                const std::shared_ptr<sync::Semaphore>& get_semaphore(const unsigned int frame_number);
                /// This will be called at the start of each frame
                void update();
                /// Multithreaded rendering happens in here
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
