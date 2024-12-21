#pragma once
#include "gx-rnd-gltf-animation.hpp"
#include "gx-rnd-gltf-armature.hpp"
#include "gx-rnd-gltf-camera.hpp"
#include "gx-rnd-gltf-light.hpp"
#include "gx-rnd-gltf-material.hpp"
#include "gx-rnd-gltf-mesh.hpp"
#include "gx-rnd-gltf-node.hpp"
#include "gx-rnd-gltf-skin.hpp"
#include "gx-rnd-gltf-texture.hpp"
#include "gx-rnd-gltf-tiny.hpp"

namespace gearoenix::render::gltf {
struct Context final {
    tinygltf::TinyGLTF context;
    tinygltf::Model data;

    Animations animations;
    Armatures armatures;
    Cameras cameras;
    Lights lights;
    Materials materials;
    Meshes meshes;
    Nodes nodes;
    Skins skins;
    Textures textures;

    Context()
        : animations(*this)
        , armatures(*this)
        , cameras(*this)
        , lights(*this)
        , materials(*this)
        , meshes(*this)
        , nodes(*this)
        , skins(*this)
        , textures(*this)
    {
    }

    ~Context() = default;
    Context(const Context&) = delete;
    Context(Context&&) = delete;
};
}
