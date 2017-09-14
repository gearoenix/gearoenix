#include "render-shader.hpp"
#include "../../core/cr-types.hpp"
#include "../../system/sys-file.hpp"
#include "../../system/sys-log.hpp"
#include "render-shader-basic.hpp"
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
