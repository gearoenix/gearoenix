#pragma once
#include "../../math/gx-math-vector-3d.hpp"
#include "gx-rnd-imgui-id.hpp"
#include "gx-rnd-imgui-input-fp.hpp"

namespace gearoenix::render::imgui {
/// Draws three labelled float inputs on one line: `X: [input]  Y: [input]  Z: [input]`.
///
/// @param v            the vector to edit in place
/// @param item_width   per-axis input width in pixels. The sentinel `0.0f` (the default)
///                     means "split the available content region equally between the
///                     three inputs, after reserving space for the X / Y / Z labels and
///                     all the item-spacing gaps". Inputs also get a lower bound of
///                     ~5 em so they stay usable on very narrow panels (text just
///                     starts to overflow the field, rather than becoming unreadably
///                     tiny).
/// @param item_format  printf-style format passed to the underlying float input.
template <typename T>
[[nodiscard]] bool show(math::Vec3<T>& v, const float item_width = 0.0f, const char* const item_format = "%.3f")
{
    const auto ptr_id = create_id_string(&v);
    constexpr auto eid_sz = std::size(ptr_id) + 2;
    constexpr auto eid_eli = eid_sz - 2;
    constexpr auto eid_di = eid_eli - 1;
    std::array<char, eid_sz> eid;
    eid[eid_di] = '.';
    eid[eid_sz - 1] = '\0';
    for (int i = 0; i < ptr_id.size() - 1; ++i) {
        eid[i] = ptr_id[i];
    }
    bool result = false;

    const auto fun = [&](T& e) constexpr {
        if constexpr (std::is_same_v<core::fp_t, T>) {
            result |= input(eid.data(), e, static_cast<core::fp_t>(0.01), static_cast<core::fp_t>(1.0), item_format);
        } else if constexpr (std::is_same_v<float, T>) {
            result |= input(eid.data(), e, static_cast<core::fp_t>(0.01), static_cast<core::fp_t>(1.0), item_format);
        } else {
            static_assert(false, "Unimplemented type.");
        }
    };

    // Compute per-axis input width. The widget is laid out as:
    //   X: [inp] [sp] Y: [inp] [sp] Z: [inp]
    // where `[sp]` is `ItemSpacing.x` (applied by each `SameLine()` call) and every
    // label is "X:" / "Y:" / "Z:" which all measure the same on any mono-ish font.
    // Total horizontal budget consumed by non-inputs:
    //   3 * label_w + 5 * sp   (label / input / sameline gaps)
    const auto& style = ImGui::GetStyle();
    const float label_w = ImGui::CalcTextSize("X:").x;
    const float sp = style.ItemSpacing.x;
    const float reserved = 3.0f * label_w + 5.0f * sp;
    const float avail = ImGui::GetContentRegionAvail().x;
    const float auto_w = (avail - reserved) / 3.0f;
    const float min_w = ImGui::GetFontSize() * 5.0f; // ~5 em: fits "-123.456" comfortably
    const float per_input = (item_width > 0.0f) ? item_width : std::max(min_w, auto_w);

    // One `PushItemWidth` covers all three inputs — each `fun(...)` call consumes it
    // as "the next item's width", and ImGui resets the slot automatically between items.
    ImGui::PushItemWidth(per_input);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("X:");
    ImGui::SameLine();
    eid[eid_eli] = 'x';
    fun(v.x);

    ImGui::SameLine();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Y:");
    ImGui::SameLine();
    eid[eid_eli] = 'y';
    fun(v.y);

    ImGui::SameLine();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Z:");
    ImGui::SameLine();
    eid[eid_eli] = 'z';
    fun(v.z);
    ImGui::PopItemWidth();

    return result;
}
}