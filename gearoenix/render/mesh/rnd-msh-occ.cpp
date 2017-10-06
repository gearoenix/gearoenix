#include "rnd-msh-occ.hpp"
#include "../../system/sys-file.hpp"
#include "../buffer/rnd-buf-mesh.hpp"
#include "../shader/rnd-shd-shader.hpp"

gearoenix::render::mesh::Occ::Occ(system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
{
    shader::Id sid;
    f->read(sid);
#ifdef DEBUG_MODE
    if (sid != 0)
        LOGF("Unexpected");
#endif
    unsigned int vec = shader::Shader::get_vertex_real_count(sid);
    buf = new buffer::Mesh(vec, f, e, c);
}

gearoenix::render::mesh::Occ::~Occ()
{
    delete buf;
}
