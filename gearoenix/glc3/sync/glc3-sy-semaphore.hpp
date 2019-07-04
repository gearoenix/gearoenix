#ifndef GEAROENIX_GLC3_SYNC_SEMAPHORE_HPP
#define GEAROENIX_GLC3_SYNC_SEMAPHORE_HPP
#include "../../core/cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_CLASS_3
#include "../../render/sync/rnd-sy-semaphore.hpp"
namespace gearoenix::glc3::sync {
class Semaphore : public render::sync::Semaphore {
public:
};
}
#endif
#endif