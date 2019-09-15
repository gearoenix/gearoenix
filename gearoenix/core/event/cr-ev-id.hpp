#ifndef GEAROENIX_CORE_EVENT_ID_HPP
#define GEAROENIX_CORE_EVENT_ID_HPP
namespace gearoenix::core::event {
enum struct Id: int {
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
	GestureMouseDrag,
	GestureTouchDrag,
    None,
};
}
#endif