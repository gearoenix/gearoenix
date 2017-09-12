#include "cr-asset-manager.hpp"
#include "../../render/camera/render-camera.hpp"
#include "../../render/light/render-light.hpp"
#include "../../render/render-engine.hpp"
#include "../../render/scene/render-scene.hpp"
#include "../../render/shader/render-shader.hpp"
#include "../../render/texture/render-texture.hpp"
#include "../../system/sys-app.hpp"
#include "../cache/file/cr-cache-file-sparse.hpp"
#include "../cache/file/cr-cache-file.hpp"
gearoenix::core::asset::Manager::Manager(system::Application* sys_app, const std::string& file)
    : sys_app(sys_app)
    , render_engine(sys_app->render_engine)
    , file(sys_app, file)
    , shaders(file)
    , cameras(file)
    , audios(file)
    , lights(file)
    , textures(file)
    , models(file)
    , scenes(file)
{
}

void gearoenix::core::asset::Manager::initialize()
{
    shaders->read_offsets();
    cameras->read_offsets();
    audios->read_offsets();
    lights->read_offsets();
    textures->read_offsets();
    models->read_offsets();
    scenes->read_offsets();
}

std::shared_ptr<gearoenix::render::shader::Shader> gearoenix::core::asset::Manager::get_shader(Id id)
{
    shaders->get(id, [this] {

    });
}

std::shared_ptr<gearoenix::render::shader::Shader> gearoenix::core::asset::Manager::get_cached_shader(Id id) const
{
}

std::shared_ptr<gearoenix::render::camera::Camera> gearoenix::core::asset::Manager::get_camera(Id id)
{
}

std::shared_ptr<gearoenix::render::camera::Camera> gearoenix::core::asset::Manager::get_cached_camera(Id id) const
{
}

std::shared_ptr<gearoenix::audio::Audio> gearoenix::core::asset::Manager::get_audio(Id id)
{
}

std::shared_ptr<gearoenix::audio::Audio> gearoenix::core::asset::Manager::get_cached_audio(Id id)
{
}

std::shared_ptr<gearoenix::render::light::Light> gearoenix::core::asset::Manager::get_light(Id id)
{
}

std::shared_ptr<gearoenix::render::light::Light> gearoenix::core::asset::Manager::get_cached_light(Id id) const
{
}

std::shared_ptr<gearoenix::render::texture::Texture> gearoenix::core::asset::Manager::get_texture(Id id)
{
}

std::shared_ptr<gearoenix::render::texture::Texture> gearoenix::core::asset::Manager::get_cached_texture(Id id) const
{
}

std::shared_ptr<gearoenix::render::model::Model> gearoenix::core::asset::Manager::get_model(Id id)
{
}

std::shared_ptr<gearoenix::render::model::Model> gearoenix::core::asset::Manager::get_cached_model(Id id) const
{
}

std::shared_ptr<gearoenix::render::scene::Scene> gearoenix::core::asset::Manager::get_scene(Id id)
{
}

std::shared_ptr<gearoenix::render::scene::Scene> gearoenix::core::asset::Manager::get_cached_scene(Id id) const
{
}
