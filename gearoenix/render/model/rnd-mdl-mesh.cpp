#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-static.hpp"
#include "../../physics/collider/phs-collider.hpp"
#include "../../system/stream/sys-stm-asset.hpp"
#include "../../system/stream/sys-stm-stream.hpp"
#include "../../system/sys-app.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../camera/rnd-cmr-orthographic.hpp"
#include "../engine/rnd-eng-engine.hpp"
#include "../light/rnd-lt-sun.hpp"
#include "../material/rnd-mat-material.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include "../widget/rnd-wdg-widget.hpp"
#include "rnd-mdl-model.hpp"
#include <iostream>
#include "rnd-mdl-mesh.hpp"

const std::shared_ptr<gearoenix::render::mesh::Mesh>& gearoenix::render::model::Mesh::get_mesh() const
{
	return msh;
}

const std::shared_ptr<gearoenix::render::material::Material>& gearoenix::render::model::Mesh::get_material() const
{
	return mat;
}

gearoenix::core::Real gearoenix::render::model::Mesh::get_radius() const
{
	return radius;
}
