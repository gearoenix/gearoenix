#include "rnd-pip-shadow-mapper.hpp"

gearoenix::render::pipeline::ShadowMapper::ShadowMapper(const std::shared_ptr<engine::Engine> e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call)
    : Pipeline(Type::ShadowMapper, e, call)
{
}

gearoenix::render::pipeline::ShadowMapper::~ShadowMapper() {}
