#include "gx-au-speaker.hpp"

gearoenix::audio::Speaker::Speaker(core::Id my_id, std::string n, const std::shared_ptr<system::stream::Stream>& f)
    : Audio(my_id, std::move(n), f)
{
}
