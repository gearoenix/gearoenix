#pragma once
#include <optional>
#include <string>

namespace gearoenix::render::imgui {
[[nodiscard]] std::pair<bool, bool> entity_name_text_input(std::string& name, std::optional<float> width = std::nullopt);
}