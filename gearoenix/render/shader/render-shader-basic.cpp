#include "../../core/cr-types.hpp"
#include "../../system/sys-file.hpp"
#include "render-shader.hpp"
gearoenix::render::shader::Shader::Shader() {}
std::shared_ptr<gearoenix::render::shader::Shader> gearoenix::render::shader::Shader::read(std::shared_ptr<system::File>& file)
{
    core::Id shader_type;
    file->read(shader_type);
    switch (shader_type) {
    case (1): {
        return break;
    }
    }
}
