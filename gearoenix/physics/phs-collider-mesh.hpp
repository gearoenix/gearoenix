#ifndef GEAROENIX_PHYSICS_COLLIDER_MESH_HPP
#define GEAROENIX_PHYSICS_COLLIDER_MESH_HPP
#include "../core/cr-types.hpp"
namespace gearoenix {
	namespace system {
		class File;
	}
	namespace physics {
		namespace collider {
			class Mesh {
			public:
				Mesh(system::File* in);
				~Mesh();
			};
		}
	}
}
#endif