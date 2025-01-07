#pragma once
#include "../../core/gx-cr-string.hpp"
#include <imgui/imgui.h>

namespace gearoenix::render::imgui {
template <typename T, typename Function>
constexpr void tree_scope(const T* const t, Function&& func)
{
    if (ImGui::TreeNode(core::String::ptr_name(t).c_str())) {
        func();
        ImGui::TreePop();
    }
}
}