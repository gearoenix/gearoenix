#ifndef GEAROENIX_AUDIO_TYPE_HPP
#define GEAROENIX_AUDIO_TYPE_HPP
#include "../core/gx-cr-types.hpp"

namespace gearoenix::audio {
enum struct Type : core::TypeId {
    Music = 1,
    Speaker = 2,
};
}
#endif
