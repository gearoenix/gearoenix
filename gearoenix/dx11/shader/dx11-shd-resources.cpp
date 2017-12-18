#include "dx11-shd-resources.hpp"
#ifdef USE_DIRECTX11
#include "../../system/sys-log.hpp"

#include "../buffer/dx11-buf-uniform.hpp"
#include "../dx11-engine.hpp"
#include "../pipeline/dx11-pip-pipeline.hpp"

gearoenix::dx11::shader::Resources::Resources(Engine* e, pipeline::Pipeline* p, buffer::Uniform* u)
    : render::shader::Resources(e, p, u)
{
}

gearoenix::dx11::shader::Resources::~Resources()
{
}

void gearoenix::dx11::shader::Resources::bind()
{
    UNIMPLEMENTED;
}

#endif
