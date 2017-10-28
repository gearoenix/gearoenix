#include "rnd-mat-solid-colored-notspeculated-nocube-shadeless-opaque.hpp"
#include "../../core/asset/cr-asset-manager.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-app.hpp"
#include "../../system/sys-file.hpp"
#include "../buffer/rnd-buf-uniform.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../light/rnd-lt-sun.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../pipeline/rnd-pip-pipeline.hpp"
#include "../rnd-engine.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include "../texture/rnd-txt-texture-2d.hpp"

const gearoenix::core::Id gearoenix::render::material::SolidColoredNotspeculatedNocubeShadelessOpaque::SHADER_ID = gearoenix::render::shader::SOLID_COLORED_NOTSPECULATED_NOCUBE_SHADELESS_OPAQUE;

gearoenix::render::material::SolidColoredNotspeculatedNocubeShadelessOpaque::Resources::Resources(Engine* e, pipeline::Pipeline* pip, buffer::Uniform* u)
    : shader::Resources(e, pip, u)
{
}

gearoenix::render::material::SolidColoredNotspeculatedNocubeShadelessOpaque::SolidColoredNotspeculatedNocubeShadelessOpaque(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> end)
    : Material(SHADER_ID, sizeof(u), e, end)
{
    u.color.read(f);
    shdrsc = reinterpret_cast<Resources*>(e->create_shader_resources(SHADER_ID, pl.get(), ub, end));
}

gearoenix::render::material::SolidColoredNotspeculatedNocubeShadelessOpaque::~SolidColoredNotspeculatedNocubeShadelessOpaque()
{
    delete shdrsc;
}

unsigned int gearoenix::render::material::SolidColoredNotspeculatedNocubeShadelessOpaque::get_vertex_elements_count() const
{
    return shader::Shader::get_vertex_real_count(SHADER_ID);
}

void gearoenix::render::material::SolidColoredNotspeculatedNocubeShadelessOpaque::update(const scene::Scene* s, const model::Model* m)
{
    u.mvp = m->get_mvp();
    ub->update(&u, sizeof(Uniform));
}

void gearoenix::render::material::SolidColoredNotspeculatedNocubeShadelessOpaque::bind()
{
    shdrsc->bind();
}
