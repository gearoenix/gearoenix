#include "rnd-shd-shader.hpp"
#include "../../core/cr-types.hpp"
#include "../../system/sys-file.hpp"
#include "../../system/sys-log.hpp"
#include "../material/rnd-mat-white.hpp"
#include "rnd-shd-basic.hpp"

const std::vector<gearoenix::render::shader::Shader::ResourceDescription>
    gearoenix::render::shader::Shader::white_rsc_des = {
        { { gearoenix::render::shader::stage::VERTEX },
            1,
            gearoenix::render::shader::Shader::ResourceDescription::UNIFORM }
    };

const std::vector<gearoenix::render::shader::VertexAttribute>
    gearoenix::render::shader::Shader::has_pos = {
        gearoenix::render::shader::VEC3F32
    };

gearoenix::render::shader::Shader::Shader() {}
std::shared_ptr<gearoenix::render::shader::Shader> gearoenix::render::shader::Shader::read(system::File* file, Engine* engine)
{
    core::Id shader_type;
    file->read(shader_type);
    LOGE("TODO");
    switch (shader_type) {
    case (1): {
        return std::shared_ptr<Shader>(new Basic(file, engine));
        break;
    }
    }
    LOGF("Unexpected!");
    return nullptr;
}

const std::vector<gearoenix::render::shader::Shader::ResourceDescription>& gearoenix::render::shader::Shader::get_resources_descriptions(Id id)
{
    switch (id) {
    case WHITE:
        return white_rsc_des;
    case DIRECTIONAL_TEXTURED_SPECULATED_NOCUBE_FULLSHADOW_OPAQUE:
        LOGF("Unimplemented!");
    }
    LOGF("Unexpected! id: " << id);
}

unsigned int gearoenix::render::shader::Shader::get_uniform_size(Id id)
{
    switch (id) {
    case WHITE:
        return sizeof(material::White::Uniform);
    case DIRECTIONAL_TEXTURED_SPECULATED_NOCUBE_FULLSHADOW_OPAQUE:
        LOGF("Unimplemented!");
    }
    LOGF("Unexpected! id: " << id);
}

const std::vector<gearoenix::render::shader::VertexAttribute>& gearoenix::render::shader::Shader::get_vertex_atributes(Id id)
{
    switch (id) {
    case WHITE:
        return has_pos;
    case DIRECTIONAL_TEXTURED_SPECULATED_NOCUBE_FULLSHADOW_OPAQUE:
        LOGF("Unimplemented!");
    }
    LOGF("Unexpected! id: " << id);
}
