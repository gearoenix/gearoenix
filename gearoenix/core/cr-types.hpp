#ifndef GEAROENIX_CORE_TYPES_HPP
#define GEAROENIX_CORE_TYPES_HPP
#include <cstdint>

#define GXPOSEPSILON 0.0001
#define GXNEGEPSILON -GXPOSEPSILON

namespace gearoenix {
	namespace core {
		typedef std::uint64_t Id;
		typedef std::uint64_t Count;
		typedef std::int64_t Offset;
		typedef float Real;

		template<class T>
		struct weak_less {
			constexpr bool operator()(const T &lhs, const T &rhs) const
			{
				if (auto lhs_l = lhs.lock()) {
					if (auto rhs_l = rhs.lock()) {
						return lhs_l.get() < rhs_l.get();
					}
					return false;
				}
				else if (auto rhs_l = rhs.lock())
				{
					return true;
				}
				return false;
			}
		};
	}
}
#endif