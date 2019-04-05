#include "rnd-cmd-buffer.hpp"

gearoenix::render::command::Buffer::~Buffer() {}

void gearoenix::render::command::Buffer::begin()
{
	recored_secondaries.clear();
	bound_resource_sets.clear();
}

void gearoenix::render::command::Buffer::end()
{}

void gearoenix::render::command::Buffer::record(const std::shared_ptr<Buffer>& o)
{
    recored_secondaries.push_back(o);
}

void gearoenix::render::command::Buffer::bind(const std::shared_ptr<pipeline::ResourceSet> &r)
{
	bound_resource_sets.push_back(r);
}