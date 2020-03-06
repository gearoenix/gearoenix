#ifndef GEAROENIX_CORE_TYPES_HPP
#define GEAROENIX_CORE_TYPES_HPP
#include <cstdint>

#define GXPOSEPSILON 0.0001f
#define GXNEGEPSILON -GXPOSEPSILON
#define GX_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170680

namespace gearoenix::core {
typedef std::uint64_t Id;
typedef std::uint8_t TypeId;
typedef std::uint64_t Count;
typedef std::int64_t Offset;
typedef float Real;
enum struct State : int { Set = 1,
    Unset = 0,
    Uninitizlized = -1 };

template <class T>
struct weak_less {
    constexpr bool operator()(const T& lhs, const T& rhs) const
    {
        if (auto lhs_l = lhs.lock()) {
            if (auto rhs_l = rhs.lock()) {
                return lhs_l.get() < rhs_l.get();
            }
            return false;
        } else if (auto rhs_l = rhs.lock()) {
            return true;
        }
        return false;
    }
};
}
#endif
