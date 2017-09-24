#include "rnd-shd-shader.hpp"
#include "../../core/cr-types.hpp"
#include "../../system/sys-file.hpp"
#include "../../system/sys-log.hpp"
#include "../material/rnd-mat-white.hpp"
#include "rnd-shd-basic.hpp"

gearoenix::render::shader::Shader::Shader() {}
std::shared_ptr<gearoenix::render::shader::Shader> gearoenix::render::shader::Shader::read(std::shared_ptr<system::File>& file, Engine* engine)
{
    core::Id shader_type;
    file->read(shader_type);
    switch (shader_type) {
    case (1): {
        return std::shared_ptr<Shader>(new Basic(file, engine));
        break;
    }
    }
    LOGF("Unexpected!");
    return nullptr;
}

const std::vector<gearoenix::render::shader::Shader::ResourceDescription>
gearoenix::render::shader::Shader::get_resources_descriptions(Id id)
{
    switch (id) {
    case WHITE:
        return { { { ResourceDescription::VERTEX }, 1, ResourceDescription::UNIFORM } };
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
