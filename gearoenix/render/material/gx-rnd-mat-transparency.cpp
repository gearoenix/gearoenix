#include "gx-rnd-mat-transparency.hpp"
#include "../../platform/gx-plt-log.hpp"
#include "../imgui/gx-rnd-imgui-id.hpp"
#include <imgui/imgui.h>

bool gearoenix::render::material::show_debug_gui(Transparency& inout)
{
    const auto id = imgui::create_id_string(&inout);
    int current_item = inout == Transparency::Opaque ? 0 : 1;
    if (ImGui::Combo(id.data(), &current_item, "Opaque\0Transparent\0")) {
        inout = current_item == 0 ? Transparency::Opaque : Transparency::Transparent;
        return true;
    }
    return false;
}

const std::string& to_string(const gearoenix::render::material::Transparency t)
{
    static const std::string opaque = "gearoenix::render::material::Transparency [ Opaque ]";
    static const std::string transparent = "gearoenix::render::material::Transparency [ Transparent ]";

    switch (t) {
    case gearoenix::render::material::Transparency::Opaque:
        return opaque;
    case gearoenix::render::material::Transparency::Transparent:
        return transparent;
    }
    GX_UNEXPECTED;
}
