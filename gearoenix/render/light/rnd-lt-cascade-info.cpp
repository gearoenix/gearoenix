#include "rnd-lt-cascade-info.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../../core/sync/cr-sync-kernel-workers.hpp"

void gearoenix::render::light::FrameCascadeInfo::initialize(const std::shared_ptr<engine::Engine>& e) noexcept
{
	kernels.resize(e->get_kernels()->get_threads_count());
}

gearoenix::render::light::CascadeInfo::CascadeInfo(const std::shared_ptr<engine::Engine>& e) noexcept
	: e(e)
	, frames(e->get_frames_count())
{
	for (FrameCascadeInfo& frame : frames) frame.initialize(e);
}
