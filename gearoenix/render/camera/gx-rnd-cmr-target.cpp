#include "gx-rnd-cmr-target.hpp"
#include "../texture/gx-rnd-txt-target.hpp"

gearoenix::render::camera::Target::Target() = default;

gearoenix::render::camera::Target::Target(Customised&& c)
    : target(std::move(c))
{
}

gearoenix::render::camera::Target::Target(Default&& d)
    : target(std::move(d))
{
}

gearoenix::render::camera::Target gearoenix::render::camera::Target::construct_customised(
    std::shared_ptr<texture::Target>&& target)
{
    return Target(Customised { .target = std::move(target) });
}

gearoenix::render::camera::Target gearoenix::render::camera::Target::construct_default(
    std::shared_ptr<texture::Target>&& first,
    std::shared_ptr<texture::Target>&& second)
{
    return Target(Default { .first = std::move(first), .second = std::move(second) });
}

bool gearoenix::render::camera::Target::is_customised() const
{
    return target.index() == CUSTOMISED_VAR_INDEX;
}

bool gearoenix::render::camera::Target::is_default() const
{
    return target.index() == DEFAULT_VAR_INDEX;
}

void gearoenix::render::camera::Target::set_customised(std::shared_ptr<texture::Target>&& customised_target)
{
    target = Customised { .target = std::move(customised_target) };
}

double gearoenix::render::camera::Target::get_aspect_ratio() const
{
    switch (target.index()) {
    case CUSTOMISED_VAR_INDEX: {
        return std::get<CUSTOMISED_VAR_INDEX>(target).target->get_aspect_ratio();
    }
    case DEFAULT_VAR_INDEX: {
        return std::get<DEFAULT_VAR_INDEX>(target).first->get_aspect_ratio();
    }
    default: {
        GX_UNEXPECTED;
    }
    }
}

gearoenix::math::Vec2<size_t> gearoenix::render::camera::Target::get_dimension() const
{
    switch (target.index()) {
    case CUSTOMISED_VAR_INDEX: {
        return std::get<CUSTOMISED_VAR_INDEX>(target).target->get_dimension();
    }
    case DEFAULT_VAR_INDEX: {
        return std::get<DEFAULT_VAR_INDEX>(target).first->get_dimension();
    }
    default: {
        GX_UNEXPECTED;
    }
    }
}

const gearoenix::render::camera::Target::Default& gearoenix::render::camera::Target::get_default() const
{
    GX_ASSERT(is_default());
    return std::get<DEFAULT_VAR_INDEX>(target);
}

const gearoenix::render::camera::Target::Customised& gearoenix::render::camera::Target::get_customised() const
{
    GX_ASSERT(is_customised());
    return std::get<CUSTOMISED_VAR_INDEX>(target);
}
