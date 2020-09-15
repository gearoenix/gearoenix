//#include "gx-vk-buf-mesh.hpp"
#ifdef USE_VULKAN
#include "../../core/sync/gx-cr-sync-end-caller.hpp"
#include "../../system/gx-sys-log.hpp"
#include "../../system/stream/gx-sys-fl-file.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../gx-vk-engine.hpp"
#include "gx-vk-buf-buffer.hpp"
#include "gx-vk-buf-manager.hpp"
#include "gx-vk-buf-sub-buffer.hpp"
#include <functional>

gearoenix::render::buffer::Mesh::Mesh(unsigned int vertex_elements_count, system::stream::Stream* format, Engine* e, std::shared_ptr<core::sync::EndCaller> c)
    : e(e)
{
    Manager* cm = e->get_cpu_buffer_manager();
    Manager* vm = e->get_gpu_buffer_manager();
    core::Count cnt;
    format->read(cnt);
    vc = cnt;
    core::Count vsec = cnt * vertex_elements_count;
    std::vector<core::Real> vd(vsec);
    unsigned int vds = vsec * sizeof(core::Real);
    vs = vds;
    for (core::Count i = 0; i < vsec; ++i)
        format->read(vd[i]);
    format->read(cnt);
    ic = cnt;
    std::vector<IndexType> idata(cnt);
    for (core::Count i = 0; i < cnt; ++i)
        format->read(idata[i]);
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

void gearoenix::render::buffer::Mesh::bind()
{
    command::Buffer* c = e->get_current_command_buffer();
    VkDeviceSize offset = vrtbuf->get_offset();
    c->bind_vertex_buffers(vrtbuf->get_buffer()->get_vulkan_data(), offset);
    offset = indbuf->get_offset();
    c->bind_index_buffer(indbuf->get_buffer()->get_vulkan_data(), offset);
}

void gearoenix::render::buffer::Mesh::draw()
{
    command::Buffer* c = e->get_current_command_buffer();
    c->draw_indexed(ic);
}
#endif
