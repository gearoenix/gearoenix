#include "rnd-pip-forward-pbr-directional-shadow.hpp"

gearoenix::render::pipeline::ForwardPbrDirectionalShadow::ForwardPbrDirectionalShadow(const std::shared_ptr<engine::Engine> e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call)
    : Pipeline(Type::ForwardPbrDirectionalShadow, e, call)
{
}

gearoenix::render::pipeline::ForwardPbrDirectionalShadow::~ForwardPbrDirectionalShadow() {}
