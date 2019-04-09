#include "rnd-pip-resource-set.hpp"
#include "../scene/rnd-scn-scene.hpp"
#include "../buffer/rnd-buf-framed-uniform.hpp"
#include "../camera/rnd-cmr-camera.hpp"
#include "../graph/node/rnd-gr-nd-forward-pbr-directional-shadow.hpp"
#include "../light/rnd-lt-light.hpp"
#include "../model/rnd-mdl-model.hpp"
#include "../mesh/rnd-msh-mesh.hpp"
#include "../material/rnd-mat-material.hpp"

gearoenix::render::pipeline::ResourceSet::~ResourceSet() {}

#define GXHELPER(c, cc) void gearoenix::render::pipeline::ResourceSet::set_##c(const std::shared_ptr<c::cc>& o) { c##_uniform_buffer = o->get_uniform_buffers()->get_buffer(); }

GXHELPER(scene, Scene)
GXHELPER(camera, Camera)
GXHELPER(light, Light)
GXHELPER(model, Model)

void gearoenix::render::pipeline::ResourceSet::set_mesh(const std::shared_ptr<mesh::Mesh>& m) {
	msh = m;
}

GXHELPER(material, Material)

#undef GXHELPER

void gearoenix::render::pipeline::ResourceSet::clean()
{
	scene_uniform_buffer = nullptr;
	camera_uniform_buffer = nullptr;
	light_uniform_buffer = nullptr;
	model_uniform_buffer = nullptr;
	material_uniform_buffer = nullptr;
	node_uniform_buffer = nullptr;
	msh = nullptr;
	color = nullptr;
	metallic_roughness = nullptr;
	normal = nullptr;
	emissive = nullptr;
}
