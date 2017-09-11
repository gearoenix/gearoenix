#include "vk-shader-manager.hpp"
#include "vk-shader-diffuse-colored.hpp"
#include "../../system/sys-file.hpp"
#include "../../system/log.hpp"

gearoenix::nufrag::render::shader::Manager::Manager(std::shared_ptr<system::File> file)
        : asset_file(file) {
    uint16_t count;
    asset_file->read(count);
//    LOGE(std::string("Shader count:") + std::to_string(count));
    for (uint16_t i = 0; i < count; ++i) {
        uint16_t id;
        uint32_t offset;
        asset_file->read(id);
        asset_file->read(offset);
        offsets[id] = offset;
//        LOGE(
//                std::string("id: ") + std::to_string(id) + std::string(" off: ") +
//                std::to_string(offset));
    }
}

std::shared_ptr<gearoenix::nufrag::render::shader::Shader>
gearoenix::nufrag::render::shader::Manager::get_shader(
        const std::shared_ptr<device::Logical> &logical_device, uint16_t id) {
    auto iter = cached.find(id);
    if (iter != cached.end() && !(iter->second.expired())) {
        LOGI(std::string("Shader ") + std::to_string(id) + " is cached!");
        return iter->second.lock();
    }
    LOGI(std::string("Shader ") + std::to_string(id) + " is not cached!");
    auto offset = offsets[id];
    asset_file->seek(offset);
    std::shared_ptr<Shader> result;
    switch (id) {
        case 1: // SHADER_DIFFUSE_COLORED
            result = std::shared_ptr<Shader>(new DiffuseColored(logical_device, asset_file));
            break;
        case 2: // SHADER_DIFFUSE_TEXTURED
        LOGF(std::string("Not implemented yet!"));
            break;
        default:
        LOGF(std::string("Unknown shader"));
    }
    cached[id] = result;
    return result;
}
