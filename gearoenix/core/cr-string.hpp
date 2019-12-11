#ifndef GEAROENIX_CORE_STRING_HPP
#define GEAROENIX_CORE_STRING_HPP

#include "event/cr-ev-button.hpp"
#include <optional>

namespace gearoenix::core {
class String {
public:
    [[nodiscard]] static bool is_character(event::button::KeyboardKeyId) noexcept;
    [[nodiscard]] static std::optional<wchar_t> to_character(event::button::KeyboardKeyId) noexcept;
};
}
#endif
