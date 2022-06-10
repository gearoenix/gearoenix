#ifndef GEAROENIX_PLATFORM_KEY_HPP
#define GEAROENIX_PLATFORM_KEY_HPP

namespace gearoenix::platform::key {
enum struct Id {
    Left,
    Right,
    Up,
    Down,
    Middle,
    Backspace,
    Back,
    Next,
    Escape,
    Enter,
    Select,
    Start,
    Home,
    Help,
    End,
    Delete,
    Insert,
    Tilda,
    Grave,
    Tab,
    CapsLock,
    RightShift,
    LeftShift,
    RightControl,
    LeftControl,
    Menu,
    Space,
    LeftSuper,
    RightSuper,
    LeftAlt,
    RightAlt,
    Less,
    Greater,
    PageDown,
    PageUp,
    Minus,
    Equal,
    LeftBracket,
    RightBracket,
    Semicolon,
    Quote,
    Comma,
    Dot,
    Slash,
    Backslash,
    ScrollLock,
    Pause,
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
    NumpadLock,
    NumpadDecimal,
    NumpadClear,
    NumpadEquals,
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
    Function,
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
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,
    VolumeUp,
    VolumeDown,
    Mute,
    Unknown,
};

enum struct Action {
    Press,
    Release,
    Click,
    Double,
    Triple,
};

void initialize_imgui_keymap() noexcept;
[[nodiscard]] int convert_mouse_to_imgui(Id) noexcept;
}

#endif