#ifndef GEAROENIX_GLES3_SYNC_SEMAPHORE_HPP
#define GEAROENIX_GLES3_SYNC_SEMAPHORE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES3
#include "../../render/sync/rnd-sy-semaphore.hpp"
namespace gearoenix::gles3::sync {
class Semaphore : public render::sync::Semaphore {
public:
};
}
#endif
#endif