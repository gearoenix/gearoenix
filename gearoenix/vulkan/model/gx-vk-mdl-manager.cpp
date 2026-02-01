#include "gx-vk-mdl-manager.hpp"
#if GX_RENDER_VULKAN_ENABLED
#include "../../physics/animation/gx-phs-anm-bone.hpp"

gearoenix::vulkan::model::Manager::Manager()
    : Singleton<Manager>(this)
    , model_uniform_indexer(render::model::Model::max_count)
    , bone_uniform_indexer(physics::animation::Bone::max_count)
{
}

gearoenix::vulkan::model::Manager::~Manager() = default;

#endif
