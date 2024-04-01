#include "gx-plt-sdl2-key.hpp"
#ifdef GX_PLATFORM_INTERFACE_SDL2
#include "../gx-plt-log.hpp"
#include <SDL2/SDL_mouse.h>
#include <boost/container/flat_map.hpp>

static const boost::container::flat_map<SDL_Keycode, gearoenix::platform::key::Id> gearoenix_sdl_keyboard_map {
    { SDLK_LEFT, gearoenix::platform::key::Id::Left },
    { SDLK_RIGHT, gearoenix::platform::key::Id::Right },
    { SDLK_UP, gearoenix::platform::key::Id::Up },
    { SDLK_DOWN, gearoenix::platform::key::Id::Down },
    { SDLK_ESCAPE, gearoenix::platform::key::Id::Escape },
    { SDLK_a, gearoenix::platform::key::Id::A },
    { SDLK_b, gearoenix::platform::key::Id::B },
    { SDLK_c, gearoenix::platform::key::Id::C },
    { SDLK_d, gearoenix::platform::key::Id::D },
    { SDLK_e, gearoenix::platform::key::Id::E },
    { SDLK_f, gearoenix::platform::key::Id::F },
    { SDLK_g, gearoenix::platform::key::Id::G },
    { SDLK_h, gearoenix::platform::key::Id::H },
    { SDLK_i, gearoenix::platform::key::Id::I },
    { SDLK_j, gearoenix::platform::key::Id::J },
    { SDLK_k, gearoenix::platform::key::Id::K },
    { SDLK_l, gearoenix::platform::key::Id::L },
    { SDLK_m, gearoenix::platform::key::Id::M },
    { SDLK_n, gearoenix::platform::key::Id::N },
    { SDLK_o, gearoenix::platform::key::Id::O },
    { SDLK_p, gearoenix::platform::key::Id::P },
    { SDLK_q, gearoenix::platform::key::Id::Q },
    { SDLK_r, gearoenix::platform::key::Id::R },
    { SDLK_s, gearoenix::platform::key::Id::S },
    { SDLK_t, gearoenix::platform::key::Id::T },
    { SDLK_u, gearoenix::platform::key::Id::U },
    { SDLK_v, gearoenix::platform::key::Id::V },
    { SDLK_w, gearoenix::platform::key::Id::W },
    { SDLK_x, gearoenix::platform::key::Id::X },
    { SDLK_y, gearoenix::platform::key::Id::Y },
    { SDLK_z, gearoenix::platform::key::Id::Z },
    { SDLK_0, gearoenix::platform::key::Id::Num0 },
    { SDLK_1, gearoenix::platform::key::Id::Num1 },
    { SDLK_2, gearoenix::platform::key::Id::Num2 },
    { SDLK_3, gearoenix::platform::key::Id::Num3 },
    { SDLK_4, gearoenix::platform::key::Id::Num4 },
    { SDLK_5, gearoenix::platform::key::Id::Num5 },
    { SDLK_6, gearoenix::platform::key::Id::Num6 },
    { SDLK_7, gearoenix::platform::key::Id::Num7 },
    { SDLK_8, gearoenix::platform::key::Id::Num8 },
    { SDLK_9, gearoenix::platform::key::Id::Num9 },
    { SDLK_KP_0, gearoenix::platform::key::Id::Numpad0 },
    { SDLK_KP_1, gearoenix::platform::key::Id::Numpad1 },
    { SDLK_KP_2, gearoenix::platform::key::Id::Numpad2 },
    { SDLK_KP_3, gearoenix::platform::key::Id::Numpad3 },
    { SDLK_KP_4, gearoenix::platform::key::Id::Numpad4 },
    { SDLK_KP_5, gearoenix::platform::key::Id::Numpad5 },
    { SDLK_KP_6, gearoenix::platform::key::Id::Numpad6 },
    { SDLK_KP_7, gearoenix::platform::key::Id::Numpad7 },
    { SDLK_KP_8, gearoenix::platform::key::Id::Numpad8 },
    { SDLK_KP_9, gearoenix::platform::key::Id::Numpad9 },
    { SDLK_SPACE, gearoenix::platform::key::Id::Space },
    { SDLK_LEFTBRACKET, gearoenix::platform::key::Id::LeftBracket },
    { SDLK_RIGHTBRACKET, gearoenix::platform::key::Id::RightBracket },
    { SDLK_BACKQUOTE, gearoenix::platform::key::Id::Tilda },
    { SDLK_BACKSPACE, gearoenix::platform::key::Id::Backspace },
    { SDLK_BACKSLASH, gearoenix::platform::key::Id::Backslash },
    { SDLK_SLASH, gearoenix::platform::key::Id::Slash },
    { SDLK_QUOTE, gearoenix::platform::key::Id::Quote },
    { SDLK_SEMICOLON, gearoenix::platform::key::Id::Semicolon },
    { SDLK_MINUS, gearoenix::platform::key::Id::Minus },
    { SDLK_EQUALS, gearoenix::platform::key::Id::Equal },
    { SDLK_TAB, gearoenix::platform::key::Id::Tab },
    { SDLK_CAPSLOCK, gearoenix::platform::key::Id::CapsLock },
    { SDLK_LSHIFT, gearoenix::platform::key::Id::LeftShift },
    { SDLK_RSHIFT, gearoenix::platform::key::Id::RightShift },
    { SDLK_LCTRL, gearoenix::platform::key::Id::LeftControl },
    { SDLK_RCTRL, gearoenix::platform::key::Id::RightControl },
    { SDLK_LALT, gearoenix::platform::key::Id::LeftAlt },
    { SDLK_RALT, gearoenix::platform::key::Id::RightAlt },
    { SDLK_MENU, gearoenix::platform::key::Id::Menu },
    { SDLK_COMMA, gearoenix::platform::key::Id::Comma },
    { SDLK_PERIOD, gearoenix::platform::key::Id::Dot },
    { SDLK_F1, gearoenix::platform::key::Id::F1 },
    { SDLK_F2, gearoenix::platform::key::Id::F2 },
    { SDLK_F3, gearoenix::platform::key::Id::F3 },
    { SDLK_F4, gearoenix::platform::key::Id::F4 },
    { SDLK_F5, gearoenix::platform::key::Id::F5 },
    { SDLK_F6, gearoenix::platform::key::Id::F6 },
    { SDLK_F7, gearoenix::platform::key::Id::F7 },
    { SDLK_F8, gearoenix::platform::key::Id::F8 },
    { SDLK_F9, gearoenix::platform::key::Id::F9 },
    { SDLK_F10, gearoenix::platform::key::Id::F10 },
    { SDLK_F11, gearoenix::platform::key::Id::F11 },
    { SDLK_F12, gearoenix::platform::key::Id::F12 },
    { SDLK_INSERT, gearoenix::platform::key::Id::Insert },
    { SDLK_DELETE, gearoenix::platform::key::Id::Delete },
    { SDLK_HOME, gearoenix::platform::key::Id::Home },
    { SDLK_END, gearoenix::platform::key::Id::End },
    { SDLK_PAGEDOWN, gearoenix::platform::key::Id::PageDown },
    { SDLK_PAGEUP, gearoenix::platform::key::Id::PageUp },
    { SDLK_SCROLLLOCK, gearoenix::platform::key::Id::ScrollLock },
    { SDLK_PAUSE, gearoenix::platform::key::Id::Pause },
    { SDLK_LESS, gearoenix::platform::key::Id::Less },
    { SDLK_GREATER, gearoenix::platform::key::Id::Greater },
    { SDLK_NUMLOCKCLEAR, gearoenix::platform::key::Id::NumpadLock },
    { SDLK_KP_DIVIDE, gearoenix::platform::key::Id::NumpadSlash },
    { SDLK_KP_MULTIPLY, gearoenix::platform::key::Id::NumpadStar },
    { SDLK_KP_MINUS, gearoenix::platform::key::Id::NumpadMinus },
    { SDLK_KP_PLUS, gearoenix::platform::key::Id::NumpadPlus },
    { SDLK_KP_ENTER, gearoenix::platform::key::Id::NumpadEnter },
    { SDLK_KP_PERIOD, gearoenix::platform::key::Id::NumpadDot },
};

gearoenix::platform::key::Id gearoenix::platform::convert_sdl_key(const SDL_Keycode key)
{
    if (const auto search = gearoenix_sdl_keyboard_map.find(key); gearoenix_sdl_keyboard_map.end() != search)
        return search->second;
    GX_LOG_E("Unhandled button: " << key << " in SDL2 platform interface.");
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