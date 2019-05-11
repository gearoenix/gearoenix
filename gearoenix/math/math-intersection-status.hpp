#ifndef GEAROEMIX_MATH_INTERSECTION_SATUTS_HPP
#define GEAROEMIX_MATH_INTERSECTION_SATUTS_HPP
namespace gearoenix
{
	namespace math
	{
		class IntersectionStatus
		{
		public:
			typedef enum {
				Above = 1,
				Cut = 2,
				In = 3,
				Out = 4,
				Tangent = 5,
				Under = 6,
			} Type;
		};
	}
}
#endif