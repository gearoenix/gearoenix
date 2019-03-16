#ifndef GEAROEMIX_MATH_VERTEX_HPP
#define GEAROEMIX_MATH_VERTEX_HPP
#include "math-vector.hpp"

namespace gearoenix {
namespace math {
    struct BasicVertex {
		Vec3 position;
		Vec3 normal;
		Vec3 tangent;
		Vec2 uv;

		BasicVertex();
		BasicVertex(const std::shared_ptr<system::stream::Stream> &f);
        void read(const std::shared_ptr<system::stream::Stream> &f);

		friend std::ostream& operator << (std::ostream &os, const BasicVertex &v)
		{
			os << "\nVertex\n{\n\tposition: " << v.position 
				<< ",\n\tnormal: " << v.normal 
				<< ",\n\ttangent: " << v.tangent 
				<< ",\n\tuv: " << v.uv << "\n}";
			return os;
		}
    };
}
}
#endif
