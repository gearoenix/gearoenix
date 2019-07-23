#ifndef GEAROENIX_RENDER_WIDGET_TYPE_HPP
#define GEAROENIX_RENDER_WIDGET_TYPE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix::render::widget {
class Type {
public:
	typedef enum : core::Id {
		BUTTON = 1,
		EDIT = 2,
		TEXT = 3,
	} Id;
};
}
#endif