#include "gx-rnd-lt-point.hpp"
#include "../../core/ecs/gx-cr-ecs-comp-type.hpp"
#include "../buffer/gx-rnd-buf-manager.hpp"
#include "gx-rnd-lt-uniform.hpp"

gearoenix::render::light::Point::Point(core::ecs::Entity* const entity, std::string&& name)
    : Light(entity, core::ecs::ComponentType::create_index(this), std::move(name), buffer::Manager::get().get_range(buffer::UniformRegionIndex::point_lights))
{
}

gearoenix::render::light::Point::~Point() = default;

void gearoenix::render::light::Point::update_uniforms()
{
    auto& ref = uniform.get_ref<GxShaderDataPointLight>();
    ref.colour = math::Vec4(colour.to<float>(), 1.0f);
    ref.position = math::Vec4(position.to<float>(), 1.0f);
}