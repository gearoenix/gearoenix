#ifndef GEAROENIX_CORE_EVENT_BUTTON_BUTTON_HPP
#define GEAROENIX_CORE_EVENT_BUTTON_BUTTON_HPP
#include "../cr-types.hpp"
#include "../../math/math-vector.hpp"

namespace gearoenix::core::event::button {

enum struct KeyboardKeyId : int {
    Left,
    Right,
    Up,
    Down,
    Backscape,
    Escape,
    Enter,
    Select,
    Start,
    Home,
    End,
    Delete,
    Tilda,
    Tab,
    CapsLock,
    RightShift,
    LeftShift,
    RightControl,
    LeftControl,
    Space,
    LeftSuper,
    RightSuper,
    LeftAlt,
    RightAlt,
    PageDown,
    PageUp,
    Minus,
    Plus,
    LeftBracket,
    RightBracket,
    Semicolon,
    Qoute,
    Comma,
    Dot,
    Slash,
    Backslash,
    Num0,
    Num1,
    Num2,
    Num3,
    Num4,
    Num5,
    Num6,
    Num7,
    Num8,
    Num9,
    Numpad0,
    Numpad1,
    Numpad2,
    Numpad3,
    Numpad4,
    Numpad5,
    Numpad6,
    Numpad7,
    Numpad8,
    Numpad9,
    NumpadSlash,
    NumpadStar,
    NumpadMinus,
    NumpadPlus,
    NumpadEnter,
    NumpadDot,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
};

enum struct KeyboardActionId : int {
	Press,
	Release,
};

enum struct MouseKeyId : int {
	Left,
	Right,
	Middle,
	Front,
	Back,
};

enum struct MouseActionId : int {
    Press,
    Release,
    Click,
    DoubleClick,
    TripleClick,
};

struct KeyboardData {
    KeyboardActionId action = KeyboardActionId::Press;
    KeyboardKeyId key = KeyboardKeyId::Escape;
};

struct MouseData {
	MouseActionId action = MouseActionId::Press;
	MouseKeyId key = MouseKeyId::Left;
	math::Vec2 position;
};

struct MouseScroll {
    math::Vec2 direction;
};

}
#endif