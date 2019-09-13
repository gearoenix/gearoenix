#ifndef GEAROENIX_RENDER_PIPELINE_TYPE_HPP
#define GEAROENIX_RENDER_PIPELINE_TYPE_HPP
namespace gearoenix::render::pipeline {
enum struct Type : unsigned int {
    DeferredPbr = 1,
    ForwardPbr = 2,
    GBufferFiller = 3,
    LightBlooming = 4,
    ShadowAccumulator = 5,
    ShadowMapper = 6,
    SSAO = 7,
    SSR = 8,
    Unlit = 9,
};
}
#endif
