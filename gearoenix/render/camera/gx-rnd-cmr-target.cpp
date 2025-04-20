#include "gx-rnd-cmr-target.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../texture/gx-rnd-txt-manager.hpp"
#include "../texture/gx-rnd-txt-target.hpp"

gearoenix::render::camera::Target::Target() = default;

gearoenix::render::camera::Target::~Target() = default;

gearoenix::render::camera::Target::Target(Customised&& c)
    : target(std::move(c))
{
}

gearoenix::render::camera::Target::Target(texture::DefaultCameraTargets&& d)
    : target(std::move(d))
{
}

gearoenix::render::camera::Target gearoenix::render::camera::Target::construct_customised(
    std::shared_ptr<texture::Target>&& target)
{
    return Target(Customised { .target = std::move(target) });
}

gearoenix::render::camera::Target gearoenix::render::camera::Target::construct_default(
    texture::DefaultCameraTargets&& targets)
{
    return Target(std::move(targets));
}

bool gearoenix::render::camera::Target::is_customised() const
{
    return target.index() == customised_var_index;
}

bool gearoenix::render::camera::Target::is_default() const
{
    return target.index() == default_var_index;
}

void gearoenix::render::camera::Target::set_customised(std::shared_ptr<texture::Target>&& customised_target)
{
    target = Customised { .target = std::move(customised_target) };
}

double gearoenix::render::camera::Target::get_aspect_ratio() const
{
    switch (target.index()) {
    case customised_var_index: {
        return std::get<customised_var_index>(target).target->get_aspect_ratio();
    }
    case default_var_index: {
        return std::get<default_var_index>(target).main->get_aspect_ratio();
    }
    default: {
        GX_UNEXPECTED;
    }
    }
}

gearoenix::math::Vec2<std::uint32_t> gearoenix::render::camera::Target::get_dimension() const
{
    switch (target.index()) {
    case customised_var_index: {
        return std::get<customised_var_index>(target).target->get_dimension();
    }
    case default_var_index: {
        return std::get<default_var_index>(target).main->get_dimension();
    }
    default: {
        GX_UNEXPECTED;
    }
    }
}

const gearoenix::render::texture::DefaultCameraTargets& gearoenix::render::camera::Target::get_default() const
{
    GX_ASSERT(is_default());
    return std::get<default_var_index>(target);
}

const gearoenix::render::camera::Target::Customised& gearoenix::render::camera::Target::get_customised() const
{
    GX_ASSERT(is_customised());
    return std::get<customised_var_index>(target);
}

void gearoenix::render::camera::Target::write(
    std::shared_ptr<platform::stream::Stream>&& s, core::job::EndCaller<>&& end) const
{
    const auto d = is_default();
    s->write_fail_debug(d);
    if (d) {
        return;
    }
    std::get<customised_var_index>(target).target->write(std::move(s), std::move(end));
}

void gearoenix::render::camera::Target::read(
    std::shared_ptr<platform::stream::Stream>&& stream,
    core::job::EndCaller<Target>&& end)
{
    if (stream->read<bool>()) {
        end.set_return(construct_default(texture::DefaultCameraTargets()));
        return;
    }
    texture::Manager::get().create_target(std::move(stream), core::job::EndCallerShared<texture::Target>([e = std::move(end)](std::shared_ptr<texture::Target>&& t) {
        e.set_return(construct_customised(std::move(t)));
    }));
}
