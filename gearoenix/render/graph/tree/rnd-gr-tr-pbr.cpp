#include "rnd-gr-tr-pbr.hpp"
#include "../../engine/rnd-eng-engine.hpp"
#include "../node/rnd-gr-nd-forward-pbr-directional-shadow.hpp"

gearoenix::render::graph::tree::Pbr::Pbr(const std::shared_ptr<engine::Engine>& e, const core::sync::EndCaller<core::sync::EndCallerIgnore>& call)
	: Tree(e)
	, in_weak_hardware(engine::Type::OPENGL_ES2 == e->get_engine_type_id())
{
	fwddirshd = std::shared_ptr<node::ForwardPbrDirectionalShadow>(new node::ForwardPbrDirectionalShadow(e, call));
}

gearoenix::render::graph::tree::Pbr::~Pbr()
{
}

void gearoenix::render::graph::tree::Pbr::record(const unsigned int kernel_index)
{
}

void gearoenix::render::graph::tree::Pbr::submit()
{
}
