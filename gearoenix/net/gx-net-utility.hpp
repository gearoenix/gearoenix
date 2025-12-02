#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace gearoenix::net {
[[nodiscard]] std::optional<std::pair<std::string, std::uint16_t>> parse_to_address_port(std::string_view a);
}