#include "gx-gl-sbm-mesh.hpp"
#ifdef GX_RENDER_OPENGL_ENABLED
#include "../gx-gl-mesh.hpp"

gearoenix::gl::submission::Mesh::Mesh(const gl::Mesh& m)
    : material(m.get_gl_material().get())
    , render_material(m.get_bound_material().get())
    , vertex_object(m.get_cached_vertex_object())
    , indices_count(m.get_cached_indices_count())
{
}

#endif
