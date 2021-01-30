#include "gx-plt-win-key.hpp"
#ifdef GX_PLATFORM_INTERFACE_WIN32
#include "../gx-plt-log.hpp"

gearoenix::platform::key::Id gearoenix::platform::convert_to_keyboard_key(const WPARAM wp, const LPARAM lp) noexcept
{
    const UINT scancode = (lp & 0x00ff0000) >> 16;
    const bool extended = (lp & 0x01000000) != 0;
    switch (wp) {
    case VK_LEFT:
        return key::Id::Left;
    case VK_RIGHT:
        return key::Id::Right;
    case VK_UP:
        return key::Id::Up;
    case VK_DOWN:
        return key::Id::Down;
    case VK_BACK:
        return key::Id::Backspace;
    case VK_ESCAPE:
        return key::Id::Escape;
    case VK_RETURN:
        return key::Id::Enter;
    case VK_HOME:
        return key::Id::Home;
    case VK_END:
        return key::Id::End;
    case VK_DELETE:
        return key::Id::Delete;
    case VK_INSERT:
        return key::Id::Insert;
    case VK_TAB:
        return key::Id::Tab;
    case VK_SHIFT:
        return VK_RSHIFT == MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX) ? key::Id::RightShift : key::Id::LeftShift;
    case VK_CONTROL:
        return extended ? key::Id::RightControl : key::Id::LeftControl;
    case VK_SPACE:
        return key::Id::Space;
    case VK_LWIN:
        return key::Id::LeftSuper;
    case VK_RWIN:
        return key::Id::RightSuper;
    case VK_MENU:
        return extended ? key::Id::RightAlt : key::Id::LeftAlt;
    case VK_PRIOR:
        return key::Id::PageUp;
    case VK_NEXT:
        return key::Id::PageDown;
    case static_cast<decltype(wp)>('A'):
        return key::Id::A;
    case static_cast<decltype(wp)>('B'):
        return key::Id::B;
    case static_cast<decltype(wp)>('C'):
        return key::Id::C;
    case static_cast<decltype(wp)>('V'):
        return key::Id::V;
    case static_cast<decltype(wp)>('W'):
        return key::Id::W;
    case static_cast<decltype(wp)>('X'):
        return key::Id::X;
    case static_cast<decltype(wp)>('Y'):
        return key::Id::Y;
    case static_cast<decltype(wp)>('Z'):
        return key::Id::Z;
    case VK_F1:
        return key::Id::F1;
    case VK_F2:
        return key::Id::F2;
    case VK_F3:
        return key::Id::F3;
    case VK_F4:
        return key::Id::F4;
    case VK_F5:
        return key::Id::F5;
    case VK_F6:
        return key::Id::F6;
    case VK_F7:
        return key::Id::F7;
    case VK_F8:
        return key::Id::F8;
    case VK_F9:
        return key::Id::F9;
    case VK_F10:
        return key::Id::F10;
    case VK_F11:
        return key::Id::F11;
    case VK_F12:
        return key::Id::F12;
    default:
        GX_LOG_E("Unexpected key conversion: " << wp);
        return key::Id::Unknown;
    }
}

#endif