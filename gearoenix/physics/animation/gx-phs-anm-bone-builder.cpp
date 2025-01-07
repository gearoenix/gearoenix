#include "gx-phs-anm-bone-builder.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-allocator.hpp"
#include "../../core/ecs/gx-cr-ecs-entity-builder.hpp"
#include "../gx-phs-transformation.hpp"
#include "gx-phs-anm-bone.hpp"

gearoenix::physics::animation::BoneBuilder::BoneBuilder(std::string&& name, Bone* parent, core::job::EndCaller<>&& end_caller)
    : builder(std::make_shared<core::ecs::EntitySharedBuilder>(std::move(name), std::move(end_caller)))
    , transform(core::ecs::construct_component<Transformation>(builder->get_name() + "-transform", parent ? parent->get_transform().get() : nullptr, builder->get_id()))
    , bone(core::ecs::construct_component<Bone>(parent, std::shared_ptr(transform), builder->get_name() + "-bone-component", builder->get_id()))
{
    builder->get_builder().add_components(std::shared_ptr(transform), std::shared_ptr(bone));
}

gearoenix::physics::animation::BoneBuilder::~BoneBuilder() = default;
