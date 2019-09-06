#ifndef GEAROENIX_CORE_EVENT_ID_HPP
#define GEAROENIX_CORE_EVENT_ID_HPP
namespace gearoenix::core::event {
enum struct Id {
    ButtonMouse,
    ButtonKeyboard,
    MovementMouse,
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
    None,
};
}
#endif