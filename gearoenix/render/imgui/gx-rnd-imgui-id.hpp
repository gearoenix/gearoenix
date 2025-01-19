#pragma once
#include "../../core/gx-cr-string.hpp"

namespace gearoenix::render::imgui {
[[nodiscard]] std::array<char, sizeof(std::uintptr_t) * 2 + 5> create_id_string(const void* ptr);
}