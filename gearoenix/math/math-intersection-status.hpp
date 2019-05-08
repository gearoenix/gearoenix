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
				Above,
				Cut,
				In,
				Out,
				Tangent,
				Under,
			} Type;
		};
	}
}
#endif