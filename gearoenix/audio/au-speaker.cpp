#include "au-speaker.hpp"

gearoenix::audio::Speaker::Speaker(core::Id my_id, const std::shared_ptr<system::stream::Stream> &f)
    : Audio(my_id, f)
{
}
