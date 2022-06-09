#include "gx-plt-key.hpp"
#include <imgui.h>

void gearoenix::platform::key::initialize_imgui_keymap() noexcept
{
    auto& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = static_cast<int>(Id::Tab);
    io.KeyMap[ImGuiKey_LeftArrow] = static_cast<int>(Id::Left);
    io.KeyMap[ImGuiKey_RightArrow] = static_cast<int>(Id::Right);
    io.KeyMap[ImGuiKey_UpArrow] = static_cast<int>(Id::Up);
    io.KeyMap[ImGuiKey_DownArrow] = static_cast<int>(Id::Down);
    io.KeyMap[ImGuiKey_PageUp] = static_cast<int>(Id::PageUp);
    io.KeyMap[ImGuiKey_PageDown] = static_cast<int>(Id::PageDown);
    io.KeyMap[ImGuiKey_Home] = static_cast<int>(Id::Home);
    io.KeyMap[ImGuiKey_End] = static_cast<int>(Id::End);
    io.KeyMap[ImGuiKey_Insert] = static_cast<int>(Id::Insert);
    io.KeyMap[ImGuiKey_Delete] = static_cast<int>(Id::Delete);
    io.KeyMap[ImGuiKey_Backspace] = static_cast<int>(Id::Backspace);
    io.KeyMap[ImGuiKey_Space] = static_cast<int>(Id::Space);
    io.KeyMap[ImGuiKey_Enter] = static_cast<int>(Id::Enter);
    io.KeyMap[ImGuiKey_Escape] = static_cast<int>(Id::Escape);
    io.KeyMap[ImGuiKey_KeyPadEnter] = static_cast<int>(Id::NumpadEnter);
    io.KeyMap[ImGuiKey_A] = static_cast<int>(Id::A);
    io.KeyMap[ImGuiKey_C] = static_cast<int>(Id::C);
    io.KeyMap[ImGuiKey_V] = static_cast<int>(Id::V);
    io.KeyMap[ImGuiKey_X] = static_cast<int>(Id::X);
    io.KeyMap[ImGuiKey_Y] = static_cast<int>(Id::Y);
    io.KeyMap[ImGuiKey_Z] = static_cast<int>(Id::Z);
}

int gearoenix::platform::key::convert_mouse_to_imgui(const Id m) noexcept
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
        return 4;
    }
}