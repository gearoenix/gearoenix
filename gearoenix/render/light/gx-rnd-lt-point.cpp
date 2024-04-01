#include "gx-rnd-lt-point.hpp"
#include "../../core/allocator/gx-cr-alc-shared-array.hpp"
#include "../../core/job/gx-cr-job-end-caller.hpp"

namespace {
gearoenix::core::allocator::SharedArray<gearoenix::render::light::Point, 16> allocator;
}

const boost::container::flat_set<std::type_index>& gearoenix::render::light::Point::get_all_the_hierarchy_types_except_component() const
{
    static const boost::container::flat_set types { std::type_index(typeid(Light)), create_this_type_index(this) };
    return types;
}

gearoenix::render::light::Point::Point(std::string&& name)
    : Light(create_this_type_index(this), std::move(name))
{
}

std::shared_ptr<gearoenix::render::light::Point> gearoenix::render::light::Point::construct(std::string&& name)
{
    auto self = allocator.make_shared(std::move(name));
    self->set_component_self(self);
    return self;
}

gearoenix::render::light::Point::~Point() = default;