#include "rnd-mdl-dynamic.hpp"

gearoenix::render::model::Dynamic::Dynamic(
    const core::Id my_id, 
    system::stream::Stream* const f, 
    engine::Engine* const e, 
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& c) noexcept
    : Model(my_id, Type::Dynamic, )
{
}
