#ifndef GEAROENIX_AUDIO_TYPE_HPP
#define GEAROENIX_AUDIO_TYPE_HPP
#include "../core/cr-types.hpp"

namespace gearoenix {
namespace audio {
    class Type {
    public:
		typedef enum : core::TypeId {
			MUSIC = 1,
			SPEAKER = 2,
		} Id;
    };
}
}
#endif
