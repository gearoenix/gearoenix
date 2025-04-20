#include "gx-rnd-scn-scene.hpp"
#include "../../core/ecs/gx-cr-ecs-world.hpp"
#include "../../physics/collider/gx-phs-cld-collider.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../light/gx-rnd-lt-light.hpp"
#include "../model/gx-rnd-mdl-model.hpp"

gearoenix::render::scene::Scene::Scene(
    const core::object_type_index_t final_type_index, const double layer, std::string&& name)
    : Component(final_type_index, std::move(name))
    , ssao_settings(0.08f, math::Numeric::epsilon<float>, 0.000f, 0.003f)
    , layer(layer)
{
}

gearoenix::render::scene::Scene::~Scene() = default;

void gearoenix::render::scene::Scene::update()
{
    record.update(entity);
}
