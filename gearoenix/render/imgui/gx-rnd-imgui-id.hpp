#pragma once
#include "../../core/gx-cr-string.hpp"

namespace gearoenix::render::imgui {
[[nodiscard]] constexpr auto create_id_string(const void* const ptr)
{
    const auto ptr_hex = core::String::ptr_to_hex_string(ptr);
    constexpr auto id_sz = ptr_hex.size() + 2;
    std::array<char, id_sz> id;
    id[0] = id[1] = '#';
    for (int i = 0; i < ptr_hex.size(); ++i) {
        id[2 + i] = ptr_hex[i];
    }
    return id;
}
}