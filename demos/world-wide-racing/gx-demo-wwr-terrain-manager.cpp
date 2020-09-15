#include "gx-demo-wwr-terrain-manager.hpp"
#include <gearoenix/core/sync/gx-cr-sync-update-manager.hpp>
#include <gearoenix/physics/collider/gx-phs-cld-collider.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-camera.hpp>
#include <gearoenix/render/camera/gx-rnd-cmr-transformation.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-dynamic.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-game.hpp>

void gearoenix::demo::wwr::TerrainManager::update() noexcept
{
    // camera_tran->get_location().x
}

gearoenix::demo::wwr::TerrainManager::TerrainManager(std::shared_ptr<render::scene::Game> s) noexcept
    : scene(std::move(s))
    , asphalt_piece(std::dynamic_pointer_cast<render::model::Dynamic>(scene->get_model("asphalt-ground")))
    , camera(scene->get_cameras().begin()->second)
    , asphalt_pieces {
        asphalt_piece,
        std::dynamic_pointer_cast<render::model::Dynamic>(asphalt_piece->clone()),
        std::dynamic_pointer_cast<render::model::Dynamic>(asphalt_piece->clone())
    }
    , camera_tran(camera->get_transformation())
{
    const auto ground_pieces_breadth = asphalt_piece->get_collider()->get_updated_box().get_diameter().x;
    math::Vec3 location(-ground_pieces_breadth, 0.0, 0.0);
    for (const auto& m : asphalt_pieces) {
        m->get_transformation()->set_location(location);
        scene->add_model(m);
        location.x += ground_pieces_breadth;
    }
    std::vector<core::Id> influenceds(ground_pieces_count + 1);
    for (std::size_t i = 0; i < ground_pieces_count; ++i) {
        influenceds[i] = asphalt_pieces[i]->get_id();
    }
    influenceds[ground_pieces_count] = camera->get_id();
    update_function_id = scene->get_e()->get_update_manager()->add(influenceds, 0.0, [this] { update(); });
}

gearoenix::demo::wwr::TerrainManager::~TerrainManager() noexcept
{
    scene->get_e()->get_update_manager()->remove(update_function_id);
}