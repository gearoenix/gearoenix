#include "gx-plt-sdl-key.hpp"
#if GX_PLATFORM_INTERFACE_SDL
#include "../gx-plt-log.hpp"
#include <boost/container/flat_map.hpp>

static const boost::container::flat_map<SDL_Scancode, gearoenix::platform::key::Id> gearoenix_sdl_keyboard_map {
    { SDL_SCANCODE_LEFT, gearoenix::platform::key::Id::Left },
    { SDL_SCANCODE_RIGHT, gearoenix::platform::key::Id::Right },
    { SDL_SCANCODE_UP, gearoenix::platform::key::Id::Up },
    { SDL_SCANCODE_DOWN, gearoenix::platform::key::Id::Down },
    { SDL_SCANCODE_ESCAPE, gearoenix::platform::key::Id::Escape },
    { SDL_SCANCODE_A, gearoenix::platform::key::Id::A },
    { SDL_SCANCODE_B, gearoenix::platform::key::Id::B },
    { SDL_SCANCODE_C, gearoenix::platform::key::Id::C },
    { SDL_SCANCODE_D, gearoenix::platform::key::Id::D },
    { SDL_SCANCODE_E, gearoenix::platform::key::Id::E },
    { SDL_SCANCODE_F, gearoenix::platform::key::Id::F },
    { SDL_SCANCODE_G, gearoenix::platform::key::Id::G },
    { SDL_SCANCODE_H, gearoenix::platform::key::Id::H },
    { SDL_SCANCODE_I, gearoenix::platform::key::Id::I },
    { SDL_SCANCODE_J, gearoenix::platform::key::Id::J },
    { SDL_SCANCODE_K, gearoenix::platform::key::Id::K },
    { SDL_SCANCODE_L, gearoenix::platform::key::Id::L },
    { SDL_SCANCODE_M, gearoenix::platform::key::Id::M },
    { SDL_SCANCODE_N, gearoenix::platform::key::Id::N },
    { SDL_SCANCODE_O, gearoenix::platform::key::Id::O },
    { SDL_SCANCODE_P, gearoenix::platform::key::Id::P },
    { SDL_SCANCODE_Q, gearoenix::platform::key::Id::Q },
    { SDL_SCANCODE_R, gearoenix::platform::key::Id::R },
    { SDL_SCANCODE_S, gearoenix::platform::key::Id::S },
    { SDL_SCANCODE_T, gearoenix::platform::key::Id::T },
    { SDL_SCANCODE_U, gearoenix::platform::key::Id::U },
    { SDL_SCANCODE_V, gearoenix::platform::key::Id::V },
    { SDL_SCANCODE_W, gearoenix::platform::key::Id::W },
    { SDL_SCANCODE_X, gearoenix::platform::key::Id::X },
    { SDL_SCANCODE_Y, gearoenix::platform::key::Id::Y },
    { SDL_SCANCODE_Z, gearoenix::platform::key::Id::Z },
    { SDL_SCANCODE_0, gearoenix::platform::key::Id::Num0 },
    { SDL_SCANCODE_1, gearoenix::platform::key::Id::Num1 },
    { SDL_SCANCODE_2, gearoenix::platform::key::Id::Num2 },
    { SDL_SCANCODE_3, gearoenix::platform::key::Id::Num3 },
    { SDL_SCANCODE_4, gearoenix::platform::key::Id::Num4 },
    { SDL_SCANCODE_5, gearoenix::platform::key::Id::Num5 },
    { SDL_SCANCODE_6, gearoenix::platform::key::Id::Num6 },
    { SDL_SCANCODE_7, gearoenix::platform::key::Id::Num7 },
    { SDL_SCANCODE_8, gearoenix::platform::key::Id::Num8 },
    { SDL_SCANCODE_9, gearoenix::platform::key::Id::Num9 },
    { SDL_SCANCODE_KP_0, gearoenix::platform::key::Id::Numpad0 },
    { SDL_SCANCODE_KP_1, gearoenix::platform::key::Id::Numpad1 },
    { SDL_SCANCODE_KP_2, gearoenix::platform::key::Id::Numpad2 },
    { SDL_SCANCODE_KP_3, gearoenix::platform::key::Id::Numpad3 },
    { SDL_SCANCODE_KP_4, gearoenix::platform::key::Id::Numpad4 },
    { SDL_SCANCODE_KP_5, gearoenix::platform::key::Id::Numpad5 },
    { SDL_SCANCODE_KP_6, gearoenix::platform::key::Id::Numpad6 },
    { SDL_SCANCODE_KP_7, gearoenix::platform::key::Id::Numpad7 },
    { SDL_SCANCODE_KP_8, gearoenix::platform::key::Id::Numpad8 },
    { SDL_SCANCODE_KP_9, gearoenix::platform::key::Id::Numpad9 },
    { SDL_SCANCODE_SPACE, gearoenix::platform::key::Id::Space },
    { SDL_SCANCODE_LEFTBRACKET, gearoenix::platform::key::Id::LeftBracket },
    { SDL_SCANCODE_RIGHTBRACKET, gearoenix::platform::key::Id::RightBracket },
    { SDL_SCANCODE_GRAVE, gearoenix::platform::key::Id::Tilda },
    { SDL_SCANCODE_BACKSPACE, gearoenix::platform::key::Id::Backspace },
    { SDL_SCANCODE_BACKSLASH, gearoenix::platform::key::Id::Backslash },
    { SDL_SCANCODE_SLASH, gearoenix::platform::key::Id::Slash },
    { SDL_SCANCODE_APOSTROPHE, gearoenix::platform::key::Id::Quote },
    { SDL_SCANCODE_SEMICOLON, gearoenix::platform::key::Id::Semicolon },
    { SDL_SCANCODE_MINUS, gearoenix::platform::key::Id::Minus },
    { SDL_SCANCODE_EQUALS, gearoenix::platform::key::Id::Equal },
    { SDL_SCANCODE_TAB, gearoenix::platform::key::Id::Tab },
    { SDL_SCANCODE_CAPSLOCK, gearoenix::platform::key::Id::CapsLock },
    { SDL_SCANCODE_LSHIFT, gearoenix::platform::key::Id::LeftShift },
    { SDL_SCANCODE_RSHIFT, gearoenix::platform::key::Id::RightShift },
    { SDL_SCANCODE_LCTRL, gearoenix::platform::key::Id::LeftControl },
    { SDL_SCANCODE_RCTRL, gearoenix::platform::key::Id::RightControl },
    { SDL_SCANCODE_LALT, gearoenix::platform::key::Id::LeftAlt },
    { SDL_SCANCODE_RALT, gearoenix::platform::key::Id::RightAlt },
    { SDL_SCANCODE_MENU, gearoenix::platform::key::Id::Menu },
    { SDL_SCANCODE_COMMA, gearoenix::platform::key::Id::Comma },
    { SDL_SCANCODE_PERIOD, gearoenix::platform::key::Id::Dot },
    { SDL_SCANCODE_F1, gearoenix::platform::key::Id::F1 },
    { SDL_SCANCODE_F2, gearoenix::platform::key::Id::F2 },
    { SDL_SCANCODE_F3, gearoenix::platform::key::Id::F3 },
    { SDL_SCANCODE_F4, gearoenix::platform::key::Id::F4 },
    { SDL_SCANCODE_F5, gearoenix::platform::key::Id::F5 },
    { SDL_SCANCODE_F6, gearoenix::platform::key::Id::F6 },
    { SDL_SCANCODE_F7, gearoenix::platform::key::Id::F7 },
    { SDL_SCANCODE_F8, gearoenix::platform::key::Id::F8 },
    { SDL_SCANCODE_F9, gearoenix::platform::key::Id::F9 },
    { SDL_SCANCODE_F10, gearoenix::platform::key::Id::F10 },
    { SDL_SCANCODE_F11, gearoenix::platform::key::Id::F11 },
    { SDL_SCANCODE_F12, gearoenix::platform::key::Id::F12 },
    { SDL_SCANCODE_INSERT, gearoenix::platform::key::Id::Insert },
    { SDL_SCANCODE_DELETE, gearoenix::platform::key::Id::Delete },
    { SDL_SCANCODE_HOME, gearoenix::platform::key::Id::Home },
    { SDL_SCANCODE_END, gearoenix::platform::key::Id::End },
    { SDL_SCANCODE_PAGEDOWN, gearoenix::platform::key::Id::PageDown },
    { SDL_SCANCODE_PAGEUP, gearoenix::platform::key::Id::PageUp },
    { SDL_SCANCODE_SCROLLLOCK, gearoenix::platform::key::Id::ScrollLock },
    { SDL_SCANCODE_PAUSE, gearoenix::platform::key::Id::Pause },
    { SDL_SCANCODE_NUMLOCKCLEAR, gearoenix::platform::key::Id::NumpadLock },
    { SDL_SCANCODE_KP_DIVIDE, gearoenix::platform::key::Id::NumpadSlash },
    { SDL_SCANCODE_KP_MULTIPLY, gearoenix::platform::key::Id::NumpadStar },
    { SDL_SCANCODE_KP_MINUS, gearoenix::platform::key::Id::NumpadMinus },
    { SDL_SCANCODE_KP_PLUS, gearoenix::platform::key::Id::NumpadPlus },
    { SDL_SCANCODE_KP_ENTER, gearoenix::platform::key::Id::NumpadEnter },
    { SDL_SCANCODE_KP_PERIOD, gearoenix::platform::key::Id::NumpadDot },
    { SDL_SCANCODE_LGUI, gearoenix::platform::key::Id::LeftSuper },
    { SDL_SCANCODE_RGUI, gearoenix::platform::key::Id::RightSuper },
};

gearoenix::platform::key::Id gearoenix::platform::convert_sdl_key(const SDL_Scancode key)
{
    if (const auto search = gearoenix_sdl_keyboard_map.find(key); gearoenix_sdl_keyboard_map.end() != search) {
        return search->second;
    }
    GX_LOG_E("Unhandled button: " << key << " scan name: " << SDL_GetScancodeName(key) << " in SDL platform interface.");
    return key::Id::Unknown;
}

gearoenix::platform::key::Id gearoenix::platform::convert_sdl_mouse_key(const Uint8 key)
{
    switch (key) {
    case SDL_BUTTON_LEFT:
        return key::Id::Left;
    case SDL_BUTTON_RIGHT:
        return key::Id::Right;
    case SDL_BUTTON_MIDDLE:
        return key::Id::Middle;
    default:
        GX_LOG_E("Unhandled mouse button, left button returned instead." << key);
        return key::Id::Left;
    }
}

#endif