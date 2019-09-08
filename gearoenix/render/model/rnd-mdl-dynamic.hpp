#ifndef GEAROENIX_RENDER_MODEL_DYNAMIC_HPP
#define GEAROENIX_RENDER_MODEL_DYNAMIC_HPP
#include "rnd-mdl-model.hpp"

namespace gearoenix::render::model {
    class Dynamic : public Model {
    public:
        Dynamic(
            core::Id my_id,
            system::stream::Stream* f,
            engine::Engine* e,
            const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
        Dynamic(
            core::Id my_id,
            engine::Engine* e,
            const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept;
    };
}
}
#endif
