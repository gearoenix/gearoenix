#include "rnd-mdl-dynamic.hpp"
#include "../../physics/collider/phs-collider.hpp"

gearoenix::render::model::Dynamic::Dynamic(
    core::Id my_id,
    system::stream::Stream* f,
    Engine* e,
    core::sync::EndCaller<core::sync::EndCallerIgnore> c,
    const RenderModel::Type t)
    : Model(my_id, t, f, e, c)
{
    initialize_axis(m);
}

gearoenix::render::model::Dynamic::~Dynamic() {}

void gearoenix::render::model::Dynamic::commit(const scene::Scene* s)
{
    //std::lock_guard<std::mutex> lg(locker);
    Model::commit(s);
    if (transformed) {
        transformed = false;
        if (nullptr != collider) {
            collider->update(m);
        }
        moccloc = m * occloc;
        commit_camera_changes(s);
        commit_lights_changes(s);
    }
}

void gearoenix::render::model::Dynamic::get_location(math::Vec3& v) const
{
    m.get_location(v);
}

void gearoenix::render::model::Dynamic::set_location(const math::Vec3& l)
{
    m.set_location(l);
    transformed = true;
}

void gearoenix::render::model::Dynamic::translate(const math::Vec3& t)
{
    //std::lock_guard<std::mutex> lg(locker);
    m.translate(t);
    for (std::pair<const core::Id, std::shared_ptr<Model>>& pmdl : children) {
        if (pmdl.second->get_render_model_type() == RenderModel::STATIC) {
            UNEXPECTED; // dynamic model can not have static model child
        } else {
            std::static_pointer_cast<Dynamic>(pmdl.second)->translate(t);
        }
    }

    transformed = true;
}

void gearoenix::render::model::Dynamic::global_scale(const core::Real s)
{
    //std::lock_guard<std::mutex> lg(locker);
    occrds *= s;
    occrdss *= s;
    m.scale4x3(s);
    for (std::pair<const core::Id, std::shared_ptr<Model>>& pmdl : children)
        if (pmdl.second->get_render_model_type() == RenderModel::STATIC) {
            UNEXPECTED; // dynamic model can not have static model child
        } else {
            std::static_pointer_cast<Dynamic>(pmdl.second)->global_scale(s);
        }
    transformed = true;
}

void gearoenix::render::model::Dynamic::local_scale(const core::Real s)
{
    //std::lock_guard<std::mutex> lg(locker);
    m.scale3x3(s);
    occrds *= s;
    occrdss *= s;
    math::Vec3 fetched;
    m.get_location(fetched);
    const math::Vec3 o = fetched;
    for (std::pair<const core::Id, std::shared_ptr<Model>>& pmdl : children) {
        if (pmdl.second->get_render_model_type() == RenderModel::STATIC) {
            UNEXPECTED; // dynamic model can not have static model child
        }
        const std::shared_ptr<Dynamic> child = std::static_pointer_cast<Dynamic>(pmdl.second);
        child->get_location(fetched);
        child->set_location((fetched - o) * s + o);
        child->local_scale(s);
    }
    transformed = true;
}

void gearoenix::render::model::Dynamic::push_model_state()
{
    //std::lock_guard<std::mutex> lg(locker);
    UNIMPLEMENTED; // other things are in the state of a model it must decide later
}

void gearoenix::render::model::Dynamic::pop_model_state()
{
    //std::lock_guard<std::mutex> lg(locker);
    UNIMPLEMENTED; // other things are in the state of a model it must decide later
}

void gearoenix::render::model::Dynamic::global_rotate(const core::Real d, const math::Vec3& axis)
{
    math::Mat4x4 r;
    Transferable::local_rotate(d, axis, r);
    m = r * m;
    for (std::pair<const core::Id, std::shared_ptr<Model>>& pmdl : children)
        if (pmdl.second->get_render_model_type() == RenderModel::STATIC) {
            UNEXPECTED; // dynamic model can not have static model child
        } else {
            std::static_pointer_cast<Dynamic>(pmdl.second)->global_rotate(r);
        }
    transformed = true;
}

void gearoenix::render::model::Dynamic::global_rotate(const math::Mat4x4& rm)
{
    Transferable::global_rotate(rm);
    m = rm * m;
    for (std::pair<const core::Id, std::shared_ptr<Model>>& pmdl : children)
        if (pmdl.second->get_render_model_type() == RenderModel::STATIC) {
            UNEXPECTED; // dynamic model can not have static model child
        } else {
            std::static_pointer_cast<Dynamic>(pmdl.second)->global_rotate(rm);
        }
    transformed = true;
}

void gearoenix::render::model::Dynamic::local_rotate(const core::Real, const math::Vec3&)
{
    UNIMPLEMENTED;
}

void gearoenix::render::model::Dynamic::local_x_rotate(const core::Real)
{
    UNIMPLEMENTED;
}

void gearoenix::render::model::Dynamic::local_y_rotate(const core::Real)
{
    UNIMPLEMENTED;
}

void gearoenix::render::model::Dynamic::local_z_rotate(const core::Real d)
{
    math::Mat4x4 r;
    Transferable::local_z_rotate(d, r);
    m = m * r;
    math::Vec3 l;
    m.get_location(l);
    for (std::pair<const core::Id, std::shared_ptr<Model>>& pmdl : children)
        if (pmdl.second->get_render_model_type() == RenderModel::STATIC) {
            UNEXPECTED; // dynamic model can not have static model child
        } else {
            physics::Transferable* cmdl = std::static_pointer_cast<Dynamic>(pmdl.second).get();
            cmdl->global_rotate(d, z_axis, l);
        }
    transformed = true;
}
