#ifndef GEAROENIX_RENDER_MATERIAL_MATERIAL_HPP
#define GEAROENIX_RENDER_MATERIAL_MATERIAL_HPP
#include "../../core/cr-static.hpp"
#include "../../core/sync/cr-sync-end-caller.hpp"
#include "../../math/math-vector.hpp"
#include "rnd-mat-translucency-mode.hpp"
#include "rnd-mat-type.hpp"
#include <optional>

namespace gearoenix::system::stream {
class Stream;
}

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::buffer {
class FramedUniform;
}

namespace gearoenix::render::texture {
class Texture2D;
}

namespace gearoenix::render::material {
class Material {
    GX_GET_CVAL_PRT(Type, material_type)
    GX_GET_UCPTR_PRT(buffer::FramedUniform, uniform_buffers)
    GX_GETSET_VAL_PRT(TranslucencyMode, translucency, TranslucencyMode::Opaque)
    GX_GET_VAL_PRT(bool, is_shadow_caster, true)
protected:
    engine::Engine* const e;
    Material(Type t, engine::Engine* e, std::size_t uniform_size) noexcept;

public:
    virtual ~Material() noexcept;
    virtual void update() noexcept = 0;
    static Material* read(system::stream::Stream* f, engine::Engine* e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept;
};
}
#endif
