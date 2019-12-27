#include "rnd-mat-material.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "rnd-mat-pbr.hpp"
#include "rnd-mat-unlit.hpp"

gearoenix::render::material::Material::Material(const Type t, engine::Engine* const e, const std::size_t uniform_size) noexcept
    : material_type(t)
    , uniform_buffers(new buffer::FramedUniform(uniform_size, e))
    , e(e)
{
}

gearoenix::render::material::Material::~Material() noexcept = default;

gearoenix::render::material::Material* gearoenix::render::material::Material::read(
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& end) noexcept
{
    const auto t = f->read<Type>();
    switch (t) {
    case Type::Pbr:
        return new Pbr(f, e, end);
    case Type::Unlit:
        return new Unlit(f, e, end);
    default:
        GXUNEXPECTED
    }
}