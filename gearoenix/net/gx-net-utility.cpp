#include "gx-net-utility.hpp"

#include <ranges>
#include <string>
#include <limits>

std::optional<std::pair<std::string, std::uint16_t>> gearoenix::net::parse_to_address_port(const std::string_view a)
{
    std::uint32_t port = 0;
    std::size_t pos = a.size();
    bool no_port = true;
    for (const char c : std::ranges::reverse_view(a)) {
        --pos;
        if (c == ':') {
            break;
        }
        if (c < '0' || c > '9') {
            return {};
        }
        port = port * 10 + (c - '0');
        if (port > std::numeric_limits<std::uint16_t>::max()) {
            return {};
        }
        no_port = false;
    }
    if (no_port) {
        return {};
    }
    return std::make_pair(std::string(a.begin(), a.begin() + pos), static_cast<std::uint16_t>(port));
}