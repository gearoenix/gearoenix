#include "vk-shader-diffuse-colored.hpp"
#include "../../system/log.hpp"
#include <string>

gearoenix::nufrag::render::shader::DiffuseColored::DiffuseColored(const std::shared_ptr<device::Logical> &logical_device, std::shared_ptr<system::File> &file): Shader(logical_device) {
    vertex = load(file);
    fragment = load(file);
}

gearoenix::nufrag::render::shader::DiffuseColored::~DiffuseColored() {
    delete_module(fragment);
    delete_module(vertex);
}

VkShaderModule gearoenix::nufrag::render::shader::DiffuseColored::get_module(int index) {
    switch(index) {
    case 0:
        return vertex;
    case 1:
        return fragment;
    default:
        LOGF(std::string("Index out of range"));
    }
    return vertex; // just for removing silly compiler errors
}
