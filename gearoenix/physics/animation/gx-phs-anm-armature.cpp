#include "gx-phs-anm-armature.hpp"
#include "../../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"

namespace {
gearoenix::core::allocator::SharedArray<gearoenix::physics::animation::Armature, 16> allocator;
}

gearoenix::physics::animation::Armature::Armature(std::string&& name)
    : core::ecs::Component(core::ecs::Component::create_this_type_index(this), std::move(name))
{
}

std::shared_ptr<gearoenix::physics::animation::Armature> gearoenix::physics::animation::Armature::construct(std::string&& name)
{
    auto self = allocator.make_shared(std::move(name));
    self->set_component_self(self);
    return self;
}

gearoenix::physics::animation::Armature::~Armature() = default;

const boost::container::flat_set<std::type_index>& gearoenix::physics::animation::Armature::get_all_the_hierarchy_types_except_component() const
{
    static const boost::container::flat_set<std::type_index> types { core::ecs::Component::create_this_type_index(this) };
    return types;
}