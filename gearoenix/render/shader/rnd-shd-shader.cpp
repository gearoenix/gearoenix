#include "rnd-shd-shader.hpp"
#include "../../core/cr-types.hpp"
#include "../../system/sys-file.hpp"
#include "../../system/sys-log.hpp"
#include "../material/rnd-mat-directional-textured-speculated-nocube-fullshadow-opaque.hpp"
#include "../material/rnd-mat-solid-colored-notspeculated-nocube-shadeless-opaque.hpp"
#include "../material/rnd-mat-white.hpp"
#include "../rnd-engine.hpp"

const std::vector<gearoenix::render::shader::stage::Id> gearoenix::render::shader::Shader::graphic_2_stage = {
    gearoenix::render::shader::stage::VERTEX,
    gearoenix::render::shader::stage::FRAGMENT
};

const std::vector<gearoenix::render::shader::ResourceDescription>
    gearoenix::render::shader::Shader::buffer_rsc_des = {
        { { gearoenix::render::shader::stage::VERTEX },
            1,
            gearoenix::render::shader::ResourceDescription::UNIFORM }
    };

const std::vector<gearoenix::render::shader::ResourceDescription>
    gearoenix::render::shader::Shader::textured_rsc_des = {
        { {
              gearoenix::render::shader::stage::VERTEX,
              gearoenix::render::shader::stage::FRAGMENT,
          },
            1,
            gearoenix::render::shader::ResourceDescription::UNIFORM },
        { { gearoenix::render::shader::stage::FRAGMENT },
            1,
            gearoenix::render::shader::ResourceDescription::SAMPER2D },
    };

const std::vector<gearoenix::render::shader::VertexAttribute>
    gearoenix::render::shader::Shader::has_pos = {
        gearoenix::render::shader::VEC3F32
    };
const std::vector<gearoenix::render::shader::VertexAttribute>
    gearoenix::render::shader::Shader::has_pos_nrm = {
        gearoenix::render::shader::VEC3F32,
        gearoenix::render::shader::VEC3F32
    };
const std::vector<gearoenix::render::shader::VertexAttribute>
    gearoenix::render::shader::Shader::has_pos_nrm_uv = {
        gearoenix::render::shader::VEC3F32,
        gearoenix::render::shader::VEC3F32,
        gearoenix::render::shader::VEC2F32
    };
const std::vector<gearoenix::render::shader::VertexAttribute>
    gearoenix::render::shader::Shader::has_pos_uv = {
        gearoenix::render::shader::VEC3F32,
        gearoenix::render::shader::VEC2F32
    };

gearoenix::render::shader::Shader::Shader() {}

gearoenix::render::shader::Shader::~Shader() {}

std::shared_ptr<gearoenix::render::shader::Shader> gearoenix::render::shader::Shader::read(core::Id sid, system::File* file, Engine* engine, std::shared_ptr<core::EndCaller> end)
{
    return std::shared_ptr<Shader>(engine->create_shader(sid, file, end));
}

const std::vector<gearoenix::render::shader::ResourceDescription>& gearoenix::render::shader::Shader::get_resources_descriptions(Id id)
{
    switch (id) {
    case WHITE_POSITION:
        return buffer_rsc_des;
    case SOLID_COLORED_NOTSPECULATED_NOCUBE_SHADELESS_OPAQUE:
        return buffer_rsc_des;
    default:
        LOGF("Unexpected! id: " << id);
    }
}

unsigned int gearoenix::render::shader::Shader::get_uniform_size(Id id)
{
    switch (id) {
    case WHITE_POSITION:
        return sizeof(material::White::Uniform);
    case SOLID_COLORED_NOTSPECULATED_NOCUBE_SHADELESS_OPAQUE:
        return sizeof(material::SolidColoredNotspeculatedNocubeShadelessOpaque::Uniform);
    default:
        LOGF("Unexpected! id: " << id);
    }
}

const std::vector<gearoenix::render::shader::VertexAttribute>& gearoenix::render::shader::Shader::get_vertex_atributes(Id id)
{
    switch (id) {
    case WHITE_POSITION:
        return has_pos;
    case SOLID_COLORED_NOTSPECULATED_NOCUBE_SHADELESS_OPAQUE:
        return has_pos;
    default:
        LOGF("Unexpected! id: " << id);
    }
}

unsigned int gearoenix::render::shader::Shader::get_vertex_real_count(Id id)
{
    switch (id) {
    case WHITE_POSITION:
    case SOLID_COLORED_NOTSPECULATED_NOCUBE_SHADELESS_OPAQUE:
        return 3;
    default:
        LOGF("Unexpected! id: " << id);
    }
}
