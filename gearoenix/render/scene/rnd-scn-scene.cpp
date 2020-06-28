#include "rnd-scn-scene.hpp"
#include "../../audio/au-audio.hpp"
#include "../../audio/au-manager.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../physics/accelerator/phs-acc-bvh.hpp"
#include "../../physics/collider/phs-cld-aabb.hpp"
#include "../../physics/constraint/phs-cns-constraint.hpp"
#include "../../physics/constraint/phs-cns-manager.hpp"
#include "../../system/sys-app.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../camera/rnd-cmr-manager.hpp"
#include "../light/rnd-lt-directional.hpp"
#include "../light/rnd-lt-manager.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../model/rnd-mdl-manager.hpp"
#include "../pipeline/rnd-pip-manager.hpp"
#include "../reflection/rnd-rfl-baked.hpp"
#include "../reflection/rnd-rfl-manager.hpp"
#include "../reflection/rnd-rfl-runtime.hpp"
#include "../shader/rnd-shd-shader.hpp"
#include "../skybox/rnd-sky-manager.hpp"

static const std::shared_ptr<gearoenix::render::camera::Camera> null_camera = nullptr;
static const std::shared_ptr<gearoenix::audio::Audio> null_audio = nullptr;
static const std::shared_ptr<gearoenix::render::light::Light> null_light = nullptr;
static const std::shared_ptr<gearoenix::render::model::Model> null_model = nullptr;
static const std::shared_ptr<gearoenix::render::skybox::Skybox> null_skybox = nullptr;
static const std::shared_ptr<gearoenix::physics::constraint::Constraint> null_constraint = nullptr;
static const std::shared_ptr<gearoenix::render::reflection::Reflection> null_reflection = nullptr;

#define GX_SCENE_INIT                                                                              \
    core::asset::Asset(my_id, core::asset::Type::Scene, std::move(name)),                          \
        scene_type(t),                                                                             \
        uniform_buffers(new buffer::FramedUniform(static_cast<unsigned int>(sizeof(Uniform)), e)), \
        static_accelerator(new gearoenix::physics::accelerator::Bvh()),                            \
        dynamic_accelerator(new gearoenix::physics::accelerator::Bvh()),                           \
        e(e),                                                                                      \
        model_manager(e->get_system_application()->get_asset_manager()->get_model_manager())

gearoenix::render::scene::Scene::Scene(
    const core::Id my_id,
    std::string name,
    const Type t,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>&) noexcept
    : GX_SCENE_INIT
{
}

gearoenix::render::scene::Scene::Scene(
    const core::Id my_id,
    std::string name,
    const Type t,
    system::stream::Stream* const f,
    engine::Engine* const e,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : GX_SCENE_INIT
{
    auto* const astmgr = e->get_system_application()->get_asset_manager();
#define GX_HELPER(x, n, cls)                                                            \
    {                                                                                   \
        auto* const mgr = astmgr->get_##x##_manager();                                  \
        std::vector<core::Id> ids;                                                      \
        f->read(ids);                                                                   \
        if (!ids.empty()) {                                                             \
            core::sync::EndCaller<n::cls> call([c](const std::shared_ptr<n::cls>&) {}); \
            for (const core::Id id : ids)                                               \
                scene_add_##x(mgr->get_gx3d(id, call));                                 \
        }                                                                               \
    }

    GX_HELPER(camera, camera, Camera)
    GX_HELPER(audio, audio, Audio)
    GX_HELPER(light, light, Light)
    GX_HELPER(model, model, Model)
    GX_HELPER(skybox, skybox, Skybox)
    GX_HELPER(reflection, reflection, Reflection)
    GX_HELPER(constraint, physics::constraint, Constraint)

#undef GX_HELPER

    GXLOGD("Number of models is: " << models.size())
    GXLOGD("Number of lights is: " << lights.size())
    GXLOGD("Number of cameras is: " << cameras.size())
    GXLOGD("Number of skyboxes is: " << skyboxs.size())
}

gearoenix::render::scene::Scene::~Scene() noexcept
{
    audios.clear();
    cameras.clear();
    constraints.clear();
    lights.clear();
    models.clear();
    skyboxs.clear();
    reflections.clear();
    static_colliders.clear();
    dynamic_colliders.clear();
    GXLOGD("Scene " << asset_id << " deleted.")
}

#ifdef GX_DEBUG_MODE
#define GX_CHECK_HELPER(x)                                          \
    if (x##s.find(id) != x##s.end()) {                              \
        GXLOGE("Error overriding of a " #x " with same Id: " << id) \
    }
#else
#define GX_CHECK_HELPER(x)
#endif

#define GX_SCENE_ADD_HELPER(x, c)                                                             \
    void gearoenix::render::scene::Scene::scene_add_##x(const std::shared_ptr<c>& o) noexcept \
    {                                                                                         \
        const core::Id id = o->get_asset_id();                                                \
        GX_CHECK_HELPER(x)                                                                    \
        x##s[id] = o;                                                                         \
    }

GX_SCENE_ADD_HELPER(camera, camera::Camera)
GX_SCENE_ADD_HELPER(audio, audio::Audio)

void gearoenix::render::scene::Scene::scene_add_light(const std::shared_ptr<light::Light>& o) noexcept
{
    const core::Id id = o->get_asset_id();
    if (o->get_light_type() == light::Type::Directional && o->get_shadow_enabled()) {
        o->set_scene(this);
    }
    GX_CHECK_HELPER(light)
    lights[id] = o;
}

GX_SCENE_ADD_HELPER(constraint, physics::constraint::Constraint)

void gearoenix::render::scene::Scene::scene_add_skybox(const std::shared_ptr<skybox::Skybox>& o) noexcept
{
    const auto id = o->get_asset_id();
    if (nullptr != o->get_baked_reflection()) {
        set_default_reflection_probe(o->get_baked_reflection());
    }
    GX_CHECK_HELPER(skybox)
    skyboxs[id] = o;
}

void gearoenix::render::scene::Scene::scene_add_reflection(const std::shared_ptr<reflection::Reflection>& o) noexcept
{
    const auto id = o->get_asset_id();
    if (o->get_reflection_type() == reflection::Type::Runtime) {
        runtime_reflections[id] = std::dynamic_pointer_cast<reflection::Runtime>(o);
    }
    GX_CHECK_HELPER(reflection)
    reflections[id] = o;
}

void gearoenix::render::scene::Scene::scene_add_model(const std::shared_ptr<model::Model>& m) noexcept
{
    const std::function<void(const std::shared_ptr<model::Model>&)> travm = [&travm, this](const std::shared_ptr<model::Model>& mdl) noexcept {
        auto& children = mdl->get_children();
        const core::Id mid = mdl->get_asset_id();
#ifdef GX_DEBUG_MODE
        if (models.find(mid) != models.end()) {
            GXLOGE("Error overriding of a model with same Id: " << mid)
        }
#endif
        mdl->set_scene(this);
        if (nullptr == mdl->get_hooked_reflection()) {
            /// The reason for this loop is, in the real scenarios, reflections count must be low and
            /// on the other hand bvh tree for reflection is superfluous
            /// in addition to that the nature of dynamic reflections are like camera,
            /// they're the queriers not the queree.
            bool colliding_reflection_not_found = true;
            for (const auto& id_reflection : reflections) {
                const auto& reflect = id_reflection.second;
                if (reflect->get_is_hooked()) {
                    if (math::IntersectionStatus::In == reflect->get_collider()->check_intersection_status(mdl->get_collider()->get_updated_box())) {
                        mdl->set_colliding_reflection(reflect.get());
                        colliding_reflection_not_found = false;
                        break;
                    }
                }
            }
            if (colliding_reflection_not_found && nullptr != default_reflection_probe) {
                mdl->set_colliding_reflection(default_reflection_probe.get());
            }
        }
        models[mid] = mdl;
        for (auto& c : children) {
            travm(c.second);
        }
    };

    travm(m);
    models_changed = true;
}

#define GX_GET_HELPER(x, c)                                                                                         \
    const std::shared_ptr<gearoenix::c>& gearoenix::render::scene::Scene::get_##x(const core::Id id) const noexcept \
    {                                                                                                               \
        const auto& find = x##s.find(id);                                                                           \
        if (x##s.end() == find) {                                                                                   \
            return null_##x;                                                                                        \
        }                                                                                                           \
        return find->second;                                                                                        \
    }                                                                                                               \
    void gearoenix::render::scene::Scene::add_##x(const std::shared_ptr<c>& m) noexcept { scene_add_##x(m); }

GX_GET_HELPER(camera, render::camera::Camera)
GX_GET_HELPER(audio, audio::Audio)
GX_GET_HELPER(light, render::light::Light)
GX_GET_HELPER(model, render::model::Model)

const std::shared_ptr<gearoenix::render::model::Model>& gearoenix::render::scene::Scene::get_model(const std::string& model_name) const noexcept
{
    return get_model(model_manager->get_cache().get_cacher().get_key(model_name));
}

GX_GET_HELPER(constraint, physics::constraint::Constraint)
GX_GET_HELPER(skybox, render::skybox::Skybox)
GX_GET_HELPER(reflection, render::reflection::Reflection)

void gearoenix::render::scene::Scene::update() noexcept
{
    if (models_changed) {
        models_changed = false;
        dynamic_colliders.clear();
        static_colliders.clear();
        for (auto& im : models) {
            auto& mdl = im.second;
            auto* const cld = mdl->get_collider();
            if (cld == nullptr || !mdl->get_enabled())
                continue;
            if (mdl->get_dynamicity()) {
                dynamic_colliders.push_back(cld);
            } else {
                static_colliders.push_back(cld);
            }
        }
        static_accelerator->reset(static_colliders);
    }
    dynamic_accelerator->reset(dynamic_colliders);

    unsigned int dirc = 0;
    for (const auto& il : lights) {
        const light::Light* const l = il.second.get();
        if (l->get_shadow_enabled())
            continue;
        const auto* const dl = dynamic_cast<const light::Directional*>(l);
        if (dl != nullptr && dirc < GX_MAX_DIRECTIONAL_LIGHTS) {
            uniform.directional_lights_color[dirc] = math::Vec4(math::Vec3<float>(dl->get_color()), 0.0f);
            uniform.directional_lights_direction[dirc] = math::Vec4(math::Vec3<float>(dl->get_direction()), 0.0f);
            ++dirc;
            continue;
        }
    }
    uniform.lights_count.x = static_cast<float>(dirc);
    uniform_buffers->update(uniform);
}

std::optional<std::pair<double, gearoenix::physics::collider::Collider*>> gearoenix::render::scene::Scene::hit(const math::Ray3& r, double d_min) const noexcept
{
    const auto hs = static_accelerator->hit(r, d_min);
    const auto d_min_l = std::nullopt == hs ? d_min : hs.value().first;
    const auto hd = dynamic_accelerator->hit(r, d_min_l);
    if (std::nullopt == hd)
        return hs;
    return hd;
}

void gearoenix::render::scene::Scene::add_shadow_cascader(const core::Id light_id) noexcept
{
    const auto search = lights.find(light_id);
    if (search == lights.end()) {
        core::sync::EndCaller<light::Light> call([](const std::shared_ptr<light::Light>&) {});
        shadow_cascader_lights[light_id] = std::dynamic_pointer_cast<light::Directional>(
            e->get_system_application()->get_asset_manager()->get_light_manager()->get_gx3d(light_id, call));
    } else {
        shadow_cascader_lights[search->first] = std::dynamic_pointer_cast<light::Directional>(search->second);
        lights.erase(search);
    }
}

void gearoenix::render::scene::Scene::remove_shadow_cascader(const core::Id light_id) noexcept
{
    const auto search = shadow_cascader_lights.find(light_id);
    if (search == shadow_cascader_lights.end())
        return;
    lights[search->first] = std::move(search->second);
    shadow_cascader_lights.erase(search);
}

void gearoenix::render::scene::Scene::set_default_reflection_probe(std::shared_ptr<reflection::Baked> rfl) noexcept
{
    for (const auto& id_mdl : models) {
        auto* const mdl = id_mdl.second.get();
        if (nullptr == mdl->get_colliding_reflection() && nullptr == mdl->get_hooked_reflection()) {
            mdl->set_colliding_reflection(rfl.get());
        }
    }
    default_reflection_probe = std::move(rfl);
}
