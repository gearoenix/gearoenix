#include "gx-demo-wwr-terrain-manager.hpp"
#include <gearoenix/render/scene/rnd-scn-game.hpp>
#include <gearoenix/render/model/rnd-mdl-dynamic.hpp>
#include <gearoenix/render/camera/rnd-cmr-camera.hpp>
#include <gearoenix/render/camera/rnd-cmr-transformation.hpp>

void gearoenix::demo::wwr::TerrainManager::update() noexcept {

}

gearoenix::demo::wwr::TerrainManager::TerrainManager(std::shared_ptr<render::scene::Game> s) noexcept
	: scene(std::move(s))
	, asphalt_piece(std::dynamic_pointer_cast<render::model::Dynamic>(scene->get_model("asphalt-ground")))
	, camera(scene->get_cameras().begin()->second)
	, asphalt_pieces {
		std::make_shared<render::model::Dynamic>(*asphalt_piece),
		std::make_shared<render::model::Dynamic>(*asphalt_piece) }
	, camera_tran(camera->get_transformation())
{
	math::Vec3 location(ground_pieces_breadth, 0.0, 0.0);
	for (const auto& m : asphalt_pieces) {
		m->get_transformation()->set_location(location);
		scene->add_model(m);
		location.x += ground_pieces_breadth;
	}
}

gearoenix::demo::wwr::TerrainManager::~TerrainManager() noexcept = default;