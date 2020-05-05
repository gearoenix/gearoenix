#ifndef GEAROENIX_CORE_EVENT_ID_HPP
#define GEAROENIX_CORE_EVENT_ID_HPP
namespace gearoenix::core::event {
enum struct Id : int {
    ButtonMouse,
    ButtonKeyboard,
    MovementMouse,
    ScrollMouse,
    SystemPause,
    SystemQuit,
    SystemReload,
    SystemResume,
    SystemUnload,
    SystemWindowSizeChange,
    InternalSystemWindowSizeChange,
    UiPressed,
    UiPressCanceled,
    UiReleased,
    UiClicked,
    UiDoubleClicked,
    UiTripleClicked,
    UiHoverIn,
    UiHoverOn,
    UiHoverOut,
    UiDraggedIn,
    UiDraggedOn,
    UiDraggedTo,
    GestureDrag2D,
    GestureDrag3D,
    GestureMouseDrag,
    GestureTouchDrag,
    None,
};
}
#endif