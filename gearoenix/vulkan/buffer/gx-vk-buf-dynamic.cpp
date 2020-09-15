//#include "gx-vk-buf-uniform.hpp"
#ifdef USE_VULKAN
#include "../../system/gx-sys-log.hpp"
#include "../../system/stream/gx-sys-fl-file.hpp"
#include "../command/gx-vk-cmd-buffer.hpp"
#include "../gx-vk-engine.hpp"
#include "gx-vk-buf-manager.hpp"
#include "gx-vk-buf-sub-buffer.hpp"

gearoenix::render::buffer::Uniform::Uniform(unsigned int us, Engine* e)
    : us(us)
    , uc(e->get_frames_count())
    , vbuf(new SubBuffer*[uc])
    , cbuf(new SubBuffer*[uc])
    , e(e)
{
    Manager* vbm = e->get_gpu_buffer_manager();
    Manager* cbm = e->get_cpu_buffer_manager();
    for (unsigned int i = 0; i < uc; ++i) {
        vbuf[i] = vbm->create_subbuffer(us);
        cbuf[i] = cbm->create_subbuffer(us);
    }
}

gearoenix::render::buffer::Uniform::~Uniform()
{
    for (unsigned int i = 0; i < uc; ++i) {
        delete vbuf[i];
        delete cbuf[i];
    }
    delete[] cbuf;
    delete[] vbuf;
}

const gearoenix::render::buffer::SubBuffer* const* gearoenix::render::buffer::Uniform::get_vbuf() const
{
    return vbuf;
}

const gearoenix::render::buffer::SubBuffer* const* gearoenix::render::buffer::Uniform::get_cbuf() const
{
    return cbuf;
}

unsigned int gearoenix::render::buffer::Uniform::get_count() const
{
    return uc;
}

unsigned int gearoenix::render::buffer::Uniform::get_size() const
{
    return us;
}

void gearoenix::render::buffer::Uniform::update(const void* data, unsigned int data_size)
{
    unsigned int frmind = e->get_current_frame_index();
    cbuf[frmind]->write(data, data_size);
}
#endif
