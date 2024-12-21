#include "gx-rnd-gltf-skin.hpp"
#include "gx-rnd-gltf-context.hpp"

gearoenix::render::gltf::Skins::Skins(const Context& context)
    : context(context)
{
}

gearoenix::render::gltf::Skins::~Skins() = default;

bool gearoenix::render::gltf::Skins::is_skin(const int node_index) const
{
    if (const auto& node = context.data.nodes[node_index]; node.mesh != -1 && node.skin != -1) {
        GX_ASSERT_D(node.children.empty());
        GX_ASSERT_D(node.camera == -1);
        GX_ASSERT_D(node.emitter == -1);
        GX_ASSERT_D(node.light == -1);
        return true;
    }
    return false;
}
