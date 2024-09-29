#include "gx-phs-cld-aabb.hpp"
#include "../../core/allocator/gx-cr-alc-shared-array.hpp"

namespace {
const auto allocator = gearoenix::core::allocator::SharedArray<gearoenix::physics::collider::Aabb3, 8192>::construct();
}

gearoenix::physics::collider::Aabb3::Aabb3(const math::Vec3<double>& upper, const math::Vec3<double>& lower, std::string&& name)
    : Component(create_this_type_index(this), std::move(name))
    , original_box(upper, lower)
    , updated_box(original_box)
{
}

gearoenix::physics::collider::Aabb3::Aabb3(const math::Aabb3<double>& original_box, std::string&& name)
    : Component(create_this_type_index(this), std::move(name))
    , original_box(original_box)
    , updated_box(original_box)
{
}

std::shared_ptr<gearoenix::physics::collider::Aabb3> gearoenix::physics::collider::Aabb3::construct(
    const math::Vec3<double>& upper,
    const math::Vec3<double>& lower,
    std::string&& name)
{
    auto self = allocator->make_shared(upper, lower, std::move(name));
    return self;
}

std::shared_ptr<gearoenix::physics::collider::Aabb3> gearoenix::physics::collider::Aabb3::construct(
    const math::Aabb3<double>& original_box,
    std::string&& name)
{
    auto self = allocator->make_shared(original_box, std::move(name));
    return self;
}

gearoenix::physics::collider::Aabb3::~Aabb3() = default;

void gearoenix::physics::collider::Aabb3::update(const math::Mat4x4<double>& transform)
{
    std::array<math::Vec3<double>, 8> corners;
    original_box.get_all_corners(corners);
    auto tmp = transform * math::Vec4(corners[0], 1.0);
    updated_box.reset(tmp.xyz());
    for (std::size_t i = 0; i < 8; ++i) {
        tmp = transform * math::Vec4(corners[i], 1.0);
        updated_box.put_without_update(tmp.xyz());
    }
    updated_box.update();
}

const boost::container::flat_set<std::type_index>& gearoenix::physics::collider::Aabb3::get_all_the_hierarchy_types_except_component() const
{
    static const boost::container::flat_set types { create_this_type_index(this) };
    return types;
}
