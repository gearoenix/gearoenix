#include "gx-plt-key.hpp"
#include "gx-plt-log.hpp"
#include <boost/container/flat_map.hpp>
#include <imgui.h>

int gearoenix::platform::key::convert_keyboard_to_imgui(const Id id)
{
    static const boost::container::flat_map<Id, ImGuiKey> map {
        { Id::Left, ImGuiKey_LeftArrow },
        { Id::Right, ImGuiKey_RightArrow },
        { Id::Up, ImGuiKey_UpArrow },
        { Id::Down, ImGuiKey_DownArrow },
        // {Id::Middle, ImGuiKey_MouseMiddle},
        { Id::Backspace, ImGuiKey_Backspace },
        { Id::Back, ImGuiKey_AppBack },
        { Id::Next, ImGuiKey_AppForward },
        { Id::Escape, ImGuiKey_Escape },
        { Id::Enter, ImGuiKey_Enter },
        { Id::Select, ImGuiKey_Menu },
        { Id::Start, ImGuiKey_LeftSuper },
        { Id::Home, ImGuiKey_Home },
        { Id::End, ImGuiKey_End },
        { Id::Delete, ImGuiKey_Delete },
        { Id::Tab, ImGuiKey_Tab },
        { Id::PageUp, ImGuiKey_PageUp },
        { Id::PageDown, ImGuiKey_PageDown },
        { Id::Insert, ImGuiKey_Insert },
        { Id::Space, ImGuiKey_Space },
        { Id::NumpadEnter, ImGuiKey_KeypadEnter },
        { Id::A, ImGuiKey_A },
        { Id::B, ImGuiKey_B },
        { Id::C, ImGuiKey_C },
        { Id::D, ImGuiKey_D },
        { Id::E, ImGuiKey_E },
        { Id::F, ImGuiKey_F },
        { Id::G, ImGuiKey_G },
        { Id::H, ImGuiKey_H },
        { Id::I, ImGuiKey_I },
        { Id::J, ImGuiKey_J },
        { Id::K, ImGuiKey_K },
        { Id::L, ImGuiKey_L },
        { Id::M, ImGuiKey_M },
        { Id::N, ImGuiKey_N },
        { Id::O, ImGuiKey_O },
        { Id::P, ImGuiKey_P },
        { Id::Q, ImGuiKey_Q },
        { Id::R, ImGuiKey_R },
        { Id::S, ImGuiKey_S },
        { Id::T, ImGuiKey_T },
        { Id::U, ImGuiKey_U },
        { Id::V, ImGuiKey_V },
        { Id::W, ImGuiKey_W },
        { Id::X, ImGuiKey_X },
        { Id::Y, ImGuiKey_Y },
        { Id::Z, ImGuiKey_Z },
        { Id::Num0, ImGuiKey_0 },
        { Id::Num1, ImGuiKey_1 },
        { Id::Num2, ImGuiKey_2 },
        { Id::Num3, ImGuiKey_3 },
        { Id::Num4, ImGuiKey_4 },
        { Id::Num5, ImGuiKey_5 },
        { Id::Num6, ImGuiKey_6 },
        { Id::Num7, ImGuiKey_7 },
        { Id::Num8, ImGuiKey_8 },
        { Id::Num9, ImGuiKey_9 },
        { Id::Numpad0, ImGuiKey_Keypad0 },
        { Id::Numpad1, ImGuiKey_Keypad1 },
        { Id::Numpad2, ImGuiKey_Keypad2 },
        { Id::Numpad3, ImGuiKey_Keypad3 },
        { Id::Numpad4, ImGuiKey_Keypad4 },
        { Id::Numpad5, ImGuiKey_Keypad5 },
        { Id::Numpad6, ImGuiKey_Keypad6 },
        { Id::Numpad7, ImGuiKey_Keypad7 },
        { Id::Numpad8, ImGuiKey_Keypad8 },
        { Id::Numpad9, ImGuiKey_Keypad9 },
    };

    if (const auto search = map.find(id); search != map.end()) {
        return search->second;
    }

    GX_UNEXPECTED;
}

int gearoenix::platform::key::convert_mouse_to_imgui(const Id m)
{
    switch (m) {
    case Id::Left:
        return 0;
    case Id::Right:
        return 1;
    case Id::Middle:
        return 2;
    case Id::Back:
        return 3;
    default:
        GX_UNEXPECTED;
    }
}