#ifndef GEAROENIX_RENDER_GRAPH_NODE_MIPMAP_GENERATOR_HPP
#define GEAROENIX_RENDER_GRAPH_NODE_MIPMAP_GENERATOR_HPP
#include "../../../core/gx-cr-pool.hpp"
#include "../../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../../math/gx-math-matrix-4d.hpp"
#include "../../material/gx-rnd-mat-translucency-mode.hpp"
#include "gx-rnd-gr-nd-node.hpp"
#include <vector>

namespace gearoenix::render::buffer {
struct Uniform;
}

namespace gearoenix::render::camera {
struct Camera;
}

namespace gearoenix::render::command {
struct Buffer;
}

namespace gearoenix::render::mesh {
struct Mesh;
}

namespace gearoenix::render::graph::node {

struct MipmapGeneratorKernel {
    const std::unique_ptr<command::Buffer> secondary_cmd;
    MipmapGeneratorKernel(engine::Engine* e, unsigned int kernel_index) noexcept;
    ~MipmapGeneratorKernel() noexcept;
};

struct MipmapGeneratorFrame {
    std::vector<std::unique_ptr<MipmapGeneratorKernel>> kernels;
    explicit MipmapGeneratorFrame(engine::Engine* e) noexcept;
    ~MipmapGeneratorFrame() noexcept;
};

struct MipmapGenerator final : public Node {
private:
    std::vector<std::unique_ptr<MipmapGeneratorFrame>> frames;
    MipmapGeneratorFrame* frame = nullptr;
    texture::Texture* const base_texture;

    void record(MipmapGeneratorKernel* kernel) noexcept;

public:
    MipmapGenerator(std::string name, texture::Texture* base_texture, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call) noexcept;
    ~MipmapGenerator() noexcept final;
    void update() noexcept final;
    void record(unsigned int kernel_index) noexcept final;
    void record_continuously(unsigned int kernel_index) noexcept final;
    void submit() noexcept final;
};
}
#endif
