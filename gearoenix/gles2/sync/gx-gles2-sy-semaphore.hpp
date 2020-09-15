#ifndef GEAROENIX_GLES2_SYNC_SEMAPHORE_HPP
#define GEAROENIX_GLES2_SYNC_SEMAPHORE_HPP
#include "../../core/gx-cr-build-configuration.hpp"
#ifdef GX_USE_OPENGL_ES2
#include "../../render/sync/gx-rnd-sy-semaphore.hpp"
namespace gearoenix {
namespace gles2 {
    namespace sync {
        class Semaphore : public render::sync::Semaphore {
        public:
        };
    }
}
}
#endif
#endif