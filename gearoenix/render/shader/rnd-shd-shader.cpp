#include "rnd-shd-shader.hpp"
#include "../../core/cr-types.hpp"
#include "../../system/sys-file.hpp"
#include "../../system/sys-log.hpp"
#include "../material/rnd-mat-directional-textured-speculated-nocube-fullshadow-opaque.hpp"
#include "../material/rnd-mat-white.hpp"
#include "rnd-shd-basic.hpp"

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

std::shared_ptr<gearoenix::render::shader::Shader> gearoenix::render::shader::Shader::read(system::File* file, Engine* engine)
{
    core::Id shader_type;
    file->read(shader_type);
    switch (shader_type) {
    case 1: {
        return std::shared_ptr<Shader>(new Basic(file, engine));
        break;
    }
    }
    LOGF("Unexpected!");
    return nullptr;
}

const std::vector<gearoenix::render::shader::ResourceDescription>& gearoenix::render::shader::Shader::get_resources_descriptions(Id id)
{
    switch (id) {
    case WHITE:
        return buffer_rsc_des;
    case DIRECTIONAL_TEXTURED_SPECULATED_NOCUBE_FULLSHADOW_OPAQUE:
        return textured_rsc_des;
    }
    LOGF("Unexpected! id: " << id);
}

unsigned int gearoenix::render::shader::Shader::get_uniform_size(Id id)
{
    switch (id) {
    case WHITE:
        return sizeof(material::White::Uniform);
    case DIRECTIONAL_TEXTURED_SPECULATED_NOCUBE_FULLSHADOW_OPAQUE:
        return sizeof(material::DirectionalTexturedSpeculatedNocubeFullshadowOpaque::Uniform);
    }
    LOGF("Unexpected! id: " << id);
}

const std::vector<gearoenix::render::shader::VertexAttribute>& gearoenix::render::shader::Shader::get_vertex_atributes(Id id)
{
    switch (id) {
    case WHITE:
        return has_pos;
    case DIRECTIONAL_TEXTURED_SPECULATED_NOCUBE_FULLSHADOW_OPAQUE:
        return has_pos_nrm_uv;
    }
    LOGF("Unexpected! id: " << id);
}

unsigned int gearoenix::render::shader::Shader::get_vertex_real_count(Id id)
{
    switch (id) {
    case WHITE:
        return 3;
    case DIRECTIONAL_TEXTURED_SPECULATED_NOCUBE_FULLSHADOW_OPAQUE:
        return 8;
    }
    LOGF("Unexpected! id: " << id);
}
