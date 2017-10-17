#include "vk-buf-mesh.hpp"
#include "../../core/cr-end-caller.hpp"
#include "../../system/sys-file.hpp"
#include "../../system/sys-log.hpp"
#include "../command/vk-cmd-buffer.hpp"
#include "../vk-engine.hpp"
#include "vk-buf-manager.hpp"
#include "vk-buf-sub-buffer.hpp"
#include <functional>

gearoenix::render::buffer::Mesh::Mesh(unsigned int vertex_elements_count, system::File* f, Engine* e, std::shared_ptr<core::EndCaller> c)
{
    Manager* cm = e->get_cpu_buffer_manager();
    Manager* vm = e->get_gpu_buffer_manager();
    core::Count cnt;
    f->read(cnt);
    vc = cnt;
    core::Count vsec = cnt * vertex_elements_count;
    std::vector<core::Real> vd(vsec);
    unsigned int vds = vsec * sizeof(core::Real);
    vs = vds;
    for (core::Count i = 0; i < vsec; ++i)
        f->read(vd[i]);
    f->read(cnt);
    ic = cnt;
    std::vector<IndexType> idata(cnt);
    for (core::Count i = 0; i < cnt; ++i)
        f->read(idata[i]);
    unsigned int ids = cnt * sizeof(IndexType);
    is = ids;
    vrtbuf = vm->create_subbuffer(vds);
    indbuf = vm->create_subbuffer(ids);
    buffer::SubBuffer* cvrtbuf = cm->create_subbuffer(vds);
    buffer::SubBuffer* cindbuf = cm->create_subbuffer(ids);
    std::function<std::function<void()>(command::Buffer*)> todo =
        [this, cvrtbuf, cindbuf, c](command::Buffer* cb) {
            vrtbuf->copy_from(cb, cvrtbuf);
            indbuf->copy_from(cb, cindbuf);
            std::function<void()> fn = [cvrtbuf, cindbuf, c] {
                delete cvrtbuf;
                delete cindbuf;
                (void)c;
            };
            return fn;
        };
    e->push_todo(todo);
}

gearoenix::render::buffer::Mesh::~Mesh()
{
    delete vrtbuf;
    delete indbuf;
}
