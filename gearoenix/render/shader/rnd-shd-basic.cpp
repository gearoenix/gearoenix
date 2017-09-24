#include "rnd-shd-basic.hpp"
#include "../../core/cr-types.hpp"
#include "../../system/sys-file.hpp"
#include "../../system/sys-log.hpp"
#include "stage/rnd-shd-stg-fragment.hpp"
#include "stage/rnd-shd-stg-vertex.hpp"

const std::vector<gearoenix::render::shader::stage::Id>
    gearoenix::render::shader::Basic::stages_ids = {
        gearoenix::render::shader::stage::VERTEX,
        gearoenix::render::shader::stage::FRAGMENT
    };

gearoenix::render::shader::Basic::Basic(system::File* file, Engine* engine)
{
    std::vector<std::uint8_t> data;
    file->read(data);
    vertex = new stage::Vertex(data, engine);
    file->read(data);
    fragment = new stage::Fragment(data, engine);
}

gearoenix::render::shader::Basic::~Basic()
{
    delete vertex;
    delete fragment;
}

const std::vector<gearoenix::render::shader::stage::Id>& gearoenix::render::shader::Basic::get_stages_ids() const
{
    return stages_ids;
}

const gearoenix::render::shader::stage::Stage* gearoenix::render::shader::Basic::get_stage(stage::Id id) const
{
    switch (id) {
    case stage::VERTEX:
        return vertex;
    case stage::FRAGMENT:
        return fragment;
    default:
        LOGF("Unexpected");
    }
}

gearoenix::render::shader::stage::Stage* gearoenix::render::shader::Basic::get_stage(stage::Id id)
{

    switch (id) {
    case stage::VERTEX:
        return vertex;
    case stage::FRAGMENT:
        return fragment;
    default:
        LOGF("Unexpected");
    }
}
