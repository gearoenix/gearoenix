#include "au-music.hpp"

gearoenix::audio::Music::Music(core::Id my_id, const std::shared_ptr<system::stream::Stream> &f)
    : Audio(my_id, f)
{
}
