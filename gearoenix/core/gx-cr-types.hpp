#ifndef GEAROENIX_CORE_TYPES_HPP
#define GEAROENIX_CORE_TYPES_HPP
#include <cstdint>

#define GX_POS_EPSILON 0.0001f
#define GX_NEG_EPSILON -GX_POS_EPSILON
#define GX_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170680

namespace gearoenix::core {

typedef std::uint64_t Id;
typedef std::uint8_t TypeId;
typedef std::uint64_t Count;
typedef std::int64_t Offset;

enum struct State : int {
    Set = 1,
    Unset = 0,
    Uninitialized = -1
};

}
#endif
