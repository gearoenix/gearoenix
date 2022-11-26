#ifndef GEAROENIX_RENDER_WIDGET_TYPE_HPP
#define GEAROENIX_RENDER_WIDGET_TYPE_HPP
#include "../../core/gx-cr-types.hpp"
namespace gearoenix::render::widget {
enum struct Type : core::TypeId {
    Button = 1,
    Edit = 2,
    Empty = 3,
    Text = 4,
    Modal = 5,
    Label = 6,
    ColorChooser = 7,
};
}
#endif