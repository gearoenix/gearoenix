#include <gearoenix/core/ecs/gx-cr-ecs-world.hpp>
#include <gearoenix/core/gx-cr-application.hpp>
#include <gearoenix/physics/constraint/gx-phs-cns-manager.hpp>
#include <gearoenix/physics/gx-phs-engine.hpp>
#include <gearoenix/physics/gx-phs-transformation.hpp>
#include <gearoenix/platform/stream/gx-plt-stm-path.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-builder.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/gx-rnd-gltf-loader.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-builder.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-scene.hpp>

struct GameApp final : gearoenix::core::Application {
    gearoenix::core::ecs::entity_id_t scene_id = gearoenix::core::ecs::INVALID_ENTITY_ID;

    explicit GameApp(gearoenix::platform::Application& plt_app)
        : Application(plt_app)
    {
        gearoenix::core::job::EndCaller end_callback([this] {
            render_engine.get_world()->get_component<gearoenix::render::scene::Scene>(scene_id)->set_enabled(true);
        });

        gearoenix::render::gltf::load(
            render_engine,
            gearoenix::platform::stream::Path::create_asset("sample.glb"),
            gearoenix::core::job::EndCaller<std::vector<std::shared_ptr<gearoenix::render::scene::Builder>>>([this, end_callback](auto&& ss) mutable {
                auto scene_builder = std::move(ss[0]);
                scene_id = scene_builder->get_id();
                auto& cb = *scene_builder->get_camera_builders().begin()->second;
                (void)render_engine.get_physics_engine()->get_constraint_manager()->create_jet_controller(
                    cb.get_entity_builder()->get_builder().get_name() + "-controller",
                    std::dynamic_pointer_cast<gearoenix::physics::Transformation>(cb.get_transformation().get_component_self().lock()),
                    std::move(end_callback));
            }),
            end_callback);
    }

    void update() override
    {
        Application::update();
    }
};

GEAROENIX_START(GameApp)