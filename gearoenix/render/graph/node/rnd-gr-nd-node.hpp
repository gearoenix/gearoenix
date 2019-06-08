#ifndef GEAROENIX_RENDER_GRAPH_NODE_NODE_HPP
#define GEAROENIX_RENDER_GRAPH_NODE_NODE_HPP
#include "../../../core/graph/cr-gr-node.hpp"
#include "../../../core/sync/cr-sync-end-caller.hpp"
#include "../../pipeline/rnd-pip-type.hpp"
#include <memory>
#include <vector>

/// Node is responsible to record command buffers.
/// It gathers all the needed data like pipeline, descriptor-sets, ... .
/// Each node has input and output links.
/// Node must provide render-data that is contain none-shared objects, like command buffer, semaphores, ...
/// There are some predefined ids for links, in addition users of engine can specify their ids
/// Node must bring a structure for its output link, and must accept a trait as its dependency
/// Node will contain strong pointer for its providers and weak pointer for its consumers
/// A node must register its consumers and signal them whenever their needed data is gathered
/// So do not forget, provider is responsible for signaling its registered consumers
/// and consumers are responsible for waiting on signal of provider for using data
/// Each node will get data of the light, scene, camera, model, ...
/// Always there is and must be a single provider for each dependency
/// But there might be several or zero consumer for a data
/// This system can be used to push independent commands on separate queue

namespace gearoenix::render {
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
namespace graph::node {
    class Node : public core::graph::Node {
    protected:
        engine::Engine* e = nullptr;
        std::vector<std::shared_ptr<texture::Texture>> input_textures;
        std::vector<std::shared_ptr<texture::Texture>> output_textures;
        texture::Target* render_target = nullptr;
        std::shared_ptr<pipeline::Pipeline> render_pipeline = nullptr;
        Node(
            engine::Engine* e,
            pipeline::Type::Id pipeline_type_id,
            unsigned int input_textures_count,
            unsigned int output_textures_count,
            const std::vector<std::string>& input_links,
            const std::vector<std::string>& output_links,
            const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;

    public:
        virtual ~Node() noexcept = default;
        virtual void set_input_texture(const std::shared_ptr<texture::Texture>& t, unsigned int index) noexcept;
        virtual void set_render_target(texture::Target* t) noexcept;
        virtual const std::shared_ptr<sync::Semaphore>& get_semaphore(unsigned int frame_number) noexcept = 0;
    };
}
}
#endif