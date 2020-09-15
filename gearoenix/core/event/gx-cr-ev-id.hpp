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
    GestureClick,
    GestureDrag2D,
    GestureDrag3D,
    GestureScale,
    GestureMouseDrag,
    GestureTouchClick,
    GestureTouchDrag,
    GestureTouchScale,
    Touch,
    None,
};
}
#endif