#include "cr-asset-manager.hpp"
#include "../../audio/au-audio.hpp"
#include "../../physics/constraint/phs-cns-placer.hpp"
#include "../../render/camera/rnd-cmr-camera.hpp"
#include "../../render/light/rnd-lt-light.hpp"
#include "../../render/mesh/rnd-msh-mesh.hpp"
#include "../../render/model/rnd-mdl-model.hpp"
#include "../../render/rnd-engine.hpp"
#include "../../render/scene/rnd-scn-scene.hpp"
#include "../../render/shader/rnd-shd-shader.hpp"
#include "../../render/texture/rnd-txt-texture.hpp"
#include "../../system/stream/sys-stm-asset.hpp"
#include "../../system/sys-app.hpp"
#include "../cache/cr-cache-cacher.hpp"
#include "../cache/file/cr-cache-file-sparse.hpp"
#include "../cache/file/cr-cache-file.hpp"
#include "../cr-end-caller.hpp"

gearoenix::core::asset::Manager::Manager(system::Application* sys_app, const std::string& name)
    : sys_app(sys_app)
    , file(new system::stream::Asset(sys_app, name))
    , shaders(new cache::Cacher())
    , cameras(new cache::file::File(file))
    , audios(new cache::file::File(file))
    , lights(new cache::file::File(file))
    , textures(new cache::file::File(file))
    , meshes(new cache::file::File(file))
    , models(new cache::file::File(file))
    , constraints(new cache::file::File(file))
    , scenes(new cache::file::File(file))
{
}

gearoenix::core::asset::Manager::~Manager()
{
    delete file;
    delete shaders;
    delete cameras;
    delete audios;
    delete lights;
    delete textures;
    delete meshes;
    delete models;
    delete constraints;
    delete scenes;

    file = nullptr;
    shaders = nullptr;
    cameras = nullptr;
    audios = nullptr;
    lights = nullptr;
    textures = nullptr;
    meshes = nullptr;
    models = nullptr;
    constraints = nullptr;
    scenes = nullptr;
}

void gearoenix::core::asset::Manager::initialize()
{
    render_engine = sys_app->get_render_engine();
    cameras->read_offsets();
    audios->read_offsets();
    lights->read_offsets();
    textures->read_offsets();
    meshes->read_offsets();
    models->read_offsets();
    constraints->read_offsets();
    scenes->read_offsets();
}

gearoenix::system::stream::Asset* gearoenix::core::asset::Manager::get_file()
{
    return file;
}

std::shared_ptr<gearoenix::render::shader::Shader> gearoenix::core::asset::Manager::get_shader(Id id, EndCaller<render::shader::Shader> end)
{
    auto result = shaders->get<render::shader::Shader>(id, [this, id, end]() -> std::shared_ptr<render::shader::Shader> {
        return std::shared_ptr<render::shader::Shader>(render::shader::Shader::read(id, nullptr, render_engine, EndCaller<EndCallerIgnore>([end](std::shared_ptr<EndCallerIgnore>) -> void {})));
    });
    end.set_data(result);
    return result;
}

std::shared_ptr<gearoenix::render::shader::Shader> gearoenix::core::asset::Manager::get_cached_shader(Id id) const
{
    return shaders->get<gearoenix::render::shader::Shader>(id);
}

std::shared_ptr<gearoenix::render::camera::Camera> gearoenix::core::asset::Manager::get_camera(Id id)
{
    std::function<std::shared_ptr<render::camera::Camera>()> fn_new = [this] {
        return std::shared_ptr<render::camera::Camera>(render::camera::Camera::read(file, sys_app));
    };
    return cameras->get(id, fn_new);
}

std::shared_ptr<gearoenix::render::camera::Camera> gearoenix::core::asset::Manager::get_cached_camera(Id id) const
{
    return cameras->get<gearoenix::render::camera::Camera>(id);
}

std::shared_ptr<gearoenix::audio::Audio> gearoenix::core::asset::Manager::get_audio(Id id)
{
    std::function<std::shared_ptr<audio::Audio>()> fn_new = [this] {
        return std::shared_ptr<gearoenix::audio::Audio>(gearoenix::audio::Audio::read(file));
    };
    return audios->get(id, fn_new);
}

std::shared_ptr<gearoenix::audio::Audio> gearoenix::core::asset::Manager::get_cached_audio(Id id)
{
    return audios->get<audio::Audio>(id);
}

std::shared_ptr<gearoenix::render::light::Light> gearoenix::core::asset::Manager::get_light(Id id)
{
    std::function<std::shared_ptr<render::light::Light>()> fn_new = [this] {
        return std::shared_ptr<render::light::Light>(render::light::Light::read(file));
    };
    return lights->get<render::light::Light>(id, fn_new);
}

std::shared_ptr<gearoenix::render::light::Light> gearoenix::core::asset::Manager::get_cached_light(Id id) const
{
    return lights->get<render::light::Light>(id);
}

std::shared_ptr<gearoenix::render::texture::Texture> gearoenix::core::asset::Manager::get_texture(Id id, EndCaller<render::texture::Texture> end)
{
    auto result = textures->get<render::texture::Texture>(id, [this, end]() -> std::shared_ptr<render::texture::Texture> {
        return std::shared_ptr<render::texture::Texture>(render::texture::Texture::read(file, render_engine, EndCaller<EndCallerIgnore>([end](std::shared_ptr<EndCallerIgnore>) -> void {})));
    });
    end.set_data(result);
    return result;
}

std::shared_ptr<gearoenix::render::texture::Texture> gearoenix::core::asset::Manager::get_cached_texture(Id id) const
{
    return textures->get<render::texture::Texture>(id);
}

std::shared_ptr<gearoenix::render::mesh::Mesh> gearoenix::core::asset::Manager::get_mesh(Id id, EndCaller<render::mesh::Mesh> end)
{
    auto result = meshes->get<render::mesh::Mesh>(id, [this, end]() -> std::shared_ptr<render::mesh::Mesh> {
        return std::shared_ptr<render::mesh::Mesh>(render::mesh::Mesh::read(file, render_engine, EndCaller<EndCallerIgnore>([end](std::shared_ptr<EndCallerIgnore>) -> void {})));
    });
    end.set_data(result);
    return result;
}

std::shared_ptr<gearoenix::render::mesh::Mesh> gearoenix::core::asset::Manager::get_cached_mesh(Id id) const
{
    return meshes->get<render::mesh::Mesh>(id);
}

std::shared_ptr<gearoenix::render::model::Model> gearoenix::core::asset::Manager::get_model(Id id, EndCaller<render::model::Model> end)
{
    auto result = models->get<render::model::Model>(id, [this, end]() -> std::shared_ptr<render::model::Model> {
        return std::shared_ptr<render::model::Model>(render::model::Model::read(file, render_engine, EndCaller<EndCallerIgnore>([end](std::shared_ptr<EndCallerIgnore>) -> void {})));
    });
    end.set_data(result);
    return result;
}

std::shared_ptr<gearoenix::render::model::Model> gearoenix::core::asset::Manager::get_cached_model(Id id) const
{
    return models->get<render::model::Model>(id);
}

std::shared_ptr<gearoenix::physics::constraint::Constraint> gearoenix::core::asset::Manager::get_constriants(Id id, EndCaller<physics::constraint::Constraint> end)
{
    auto result = constraints->get<physics::constraint::Constraint>(id, [this, end]() -> std::shared_ptr<physics::constraint::Constraint> {
        return std::shared_ptr<physics::constraint::Constraint>(physics::constraint::Constraint::read(file, render_engine, EndCaller<EndCallerIgnore>([end](std::shared_ptr<EndCallerIgnore>) -> void {})));
    });
    end.set_data(result);
    return result;
}

std::shared_ptr<gearoenix::physics::constraint::Constraint> gearoenix::core::asset::Manager::get_cached_constraints(Id id) const
{
    return constraints->get<physics::constraint::Constraint>(id);
}

std::shared_ptr<gearoenix::render::scene::Scene> gearoenix::core::asset::Manager::get_scene(Id id, EndCaller<render::scene::Scene> end)
{
    auto result = scenes->get<render::scene::Scene>(id, [this, end]() -> std::shared_ptr<render::scene::Scene> {
        return std::shared_ptr<render::scene::Scene>(render::scene::Scene::read(file, render_engine, EndCaller<EndCallerIgnore>([end](std::shared_ptr<EndCallerIgnore>) -> void {})));
    });
    end.set_data(result);
    return result;
}

std::shared_ptr<gearoenix::render::scene::Scene> gearoenix::core::asset::Manager::get_cached_scene(Id id) const
{
    return scenes->get<render::scene::Scene>(id);
}
