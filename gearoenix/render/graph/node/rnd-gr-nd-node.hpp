#ifndef GEAROENIX_RENDER_GRAPH_NODE_NODE_HPP
#define GEAROENIX_RENDER_GRAPH_NODE_NODE_HPP
#include "../../../core/graph/cr-gr-node.hpp"
#include "../../../core/sync/cr-sync-end-caller.hpp"
#include "../../pipeline/rnd-pip-type.hpp"
#include <memory>
#include <vector>

/// Node is reponsible to record command buffers.
/// It gathers all the needed data like pipeline, descriptor-sets, ... .
/// Each node has input and output links.
/// Node must provide render-data that is contain none-shared objects, like command buffer, semaphores, ...
/// There are some predifined ids for links, in addition users of engine can specify their ids
/// Node must bring a structure for its output link, and must accept a trait as its dependancy
/// Node will contain strong pointer for its providers and weak pointer for its consumers
/// A node must register itself a cunsumer it is, gather its data when provider provided its data.
/// A node must register its cunsumers and signal them whenever their needed data is gathered
/// So do not forget provider is responsible for signaling its registered consumers
/// and consumers are responsible for waiting on signal of provider for using data
/// Each node will get data of the light, scene, camera, model, ...
/// Always there is and must be a single provider for each dependancy
/// But there might be several or zero consumer for a data
/// This system can be used to push independant commands on separate queue

namespace gearoenix {
namespace render {
    namespace engine {
        class Engine;
    }
    namespace pipeline {
        class Pipeline;
    }
    namespace sync {
        class Semaphore;
    }
    namespace texture {
        class Texture;
        class Target;
    }
    namespace graph {
        namespace node {
            class Node : public core::graph::Node {
            protected:
                const std::shared_ptr<engine::Engine> e;
                std::vector<std::shared_ptr<texture::Texture>> input_textures;
                std::vector<std::shared_ptr<texture::Texture>> output_textures;
                texture::Target* render_target = nullptr;
                std::shared_ptr<pipeline::Pipeline> render_pipeline = nullptr;
                Node(
                    const std::shared_ptr<engine::Engine>& e,
                    const pipeline::Type::Id pipeline_type_id,
                    const unsigned int input_textures_count,
                    const unsigned int output_textures_count,
                    const std::vector<std::string>& input_links,
                    const std::vector<std::string>& output_links,
                    const core::sync::EndCaller<core::sync::EndCallerIgnore>& call);

            public:
                virtual ~Node();
                virtual void set_input_texture(const std::shared_ptr<texture::Texture>& t, const unsigned int index);
                virtual void set_render_target(texture::Target* t);
                virtual const std::shared_ptr<sync::Semaphore>& get_semaphore(const unsigned int frame_number) = 0;
            };
        }
    }
}
}
#endif