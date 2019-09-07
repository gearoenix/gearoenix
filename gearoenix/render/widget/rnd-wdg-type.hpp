#ifndef GEAROENIX_RENDER_WIDGET_TYPE_HPP
#define GEAROENIX_RENDER_WIDGET_TYPE_HPP
#include "../../core/cr-types.hpp"
namespace gearoenix::render::widget {
enum struct Type : core::TypeId {
	Button = 1,
	Edit = 2,
	Text = 3,
    Modal = 4,
    Label = 5,
    ColorChooser = 6,
};
}
#endif