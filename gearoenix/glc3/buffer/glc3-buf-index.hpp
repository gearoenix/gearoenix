#ifndef GEAROENIX_GLC3_BUFFER_INDEX_HPP
#define GEAROENIX_GLC3_BUFFER_INDEX_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../gl/gl-types.hpp"
#include "../../render/buffer/rnd-buf-static.hpp"
#include <vector>

namespace gearoenix::glc3::engine {
class Engine;
}

namespace gearoenix::glc3::buffer {
class Index final : public render::buffer::Static {
public:
private:
    gl::uint bo = 0;
    gl::sizei count = 0;

public:
    Index(
        const std::vector<std::uint32_t>& indices,
        engine::Engine* e,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    ~Index() noexcept final;
    void draw() const noexcept;
};
}

#endif
#endif
