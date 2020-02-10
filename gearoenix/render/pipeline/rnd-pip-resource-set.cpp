#include "rnd-pip-resource-set.hpp"
#include "rnd-pip-pipeline.hpp"

gearoenix::render::pipeline::ResourceSet::ResourceSet(std::shared_ptr<Pipeline const> pip) noexcept
    : pip(std::move(pip))
{
}

gearoenix::render::pipeline::ResourceSet::~ResourceSet() noexcept = default;
