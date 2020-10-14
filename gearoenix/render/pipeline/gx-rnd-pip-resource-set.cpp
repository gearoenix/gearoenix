#include "gx-rnd-pip-resource-set.hpp"
#include "gx-rnd-pip-pipeline.hpp"

gearoenix::render::pipeline::ResourceSet::ResourceSet(std::shared_ptr<Pipeline const> pip) noexcept
    : pip(std::move(pip))
{
}

gearoenix::render::pipeline::ResourceSet::~ResourceSet() noexcept = default;
