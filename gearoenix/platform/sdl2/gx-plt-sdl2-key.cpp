#include "gx-plt-sdl2-key.hpp"
#ifdef GX_PLATFORM_INTERFACE_SDL2
#include "../gx-plt-log.hpp"
#include <X11/Xlib.h>

gearoenix::platform::key::Id gearoenix::platform::convert_mouse_to_key(const unsigned int b) noexcept
{
    switch (b) {
    case Button1:
        return key::Id::Left;
    case Button2:
        return key::Id::Middle;
    case Button3:
        return key::Id::Right;
    case Button4:
        return key::Id::Back;
    case Button5:
        return key::Id::Next;
    default:
        GX_LOG_E("Unexpected mouse button: " << b)
        return key::Id::Unknown;
    }
}
#endif