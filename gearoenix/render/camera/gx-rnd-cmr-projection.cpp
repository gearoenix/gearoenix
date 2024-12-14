#include "gx-rnd-cmr-projection.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"

const gearoenix::render::camera::ProjectionData::Perspective& gearoenix::render::camera::ProjectionData::get_perspective() const
{
    GX_ASSERT(perspective_index == index);
    return data.perspective;
}

gearoenix::render::camera::ProjectionData::Perspective& gearoenix::render::camera::ProjectionData::get_perspective()
{
    GX_ASSERT(perspective_index == index);
    return data.perspective;
}

const gearoenix::render::camera::ProjectionData::Orthographic& gearoenix::render::camera::ProjectionData::get_orthographic() const
{
    GX_ASSERT(orthographic_index == index);
    return data.orthographic;
}

gearoenix::render::camera::ProjectionData::Orthographic& gearoenix::render::camera::ProjectionData::get_orthographic()
{
    GX_ASSERT(orthographic_index == index);
    return data.orthographic;
}

void gearoenix::render::camera::ProjectionData::write(platform::stream::Stream& s) const
{
    s.write_fail_debug(index);
    switch (index) {
    case perspective_index:
        s.write_fail_debug(data.perspective.field_of_view_y);
        break;
    case orthographic_index:
        s.write_fail_debug(data.orthographic.scale);
        break;
    default: {
        GX_UNEXPECTED;
    }
    }
}

void gearoenix::render::camera::ProjectionData::read(platform::stream::Stream& s)
{
    s.read(index);
    switch (index) {
    case perspective_index:
        s.read(data.perspective.field_of_view_y);
        break;
    case orthographic_index:
        s.read(data.orthographic.scale);
        break;
    default: {
        GX_UNEXPECTED;
    }
    }
}
