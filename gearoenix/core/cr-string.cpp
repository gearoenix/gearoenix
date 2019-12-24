#include "cr-string.hpp"

bool gearoenix::core::String::is_character(const event::button::KeyboardKeyId ki) noexcept
{
    return to_character(ki).has_value();
}

std::optional<wchar_t> gearoenix::core::String::to_character(const event::button::KeyboardKeyId ki, const bool sp, const Language l) noexcept
{
    if (l == Language::English) {
        switch (ki) {
        case event::button::KeyboardKeyId::Enter:
        case event::button::KeyboardKeyId::NumpadEnter:
            return '\n';
        case event::button::KeyboardKeyId::Tilda:
            return sp ? '`' : '~';
        case event::button::KeyboardKeyId::Less:
            return '<';
        case event::button::KeyboardKeyId::Greater:
            return '>';
        case event::button::KeyboardKeyId::Tab:
            return '\t';
        case event::button::KeyboardKeyId::Space:
            return ' ';
        case event::button::KeyboardKeyId::Minus:
            return sp ? '-' : '_';
        case event::button::KeyboardKeyId::NumpadMinus:
            return '-';
        case event::button::KeyboardKeyId::Equal:
            return sp ? '=' : '+';
        case event::button::KeyboardKeyId::LeftBracket:
            return sp ? '[' : '{';
        case event::button::KeyboardKeyId::RightBracket:
            return sp ? ']' : '}';
        case event::button::KeyboardKeyId::Semicolon:
            return sp ? ';' : ':';
        case event::button::KeyboardKeyId::Quote:
            return sp ? '\'' : '"';
        case event::button::KeyboardKeyId::Comma:
            return sp ? ',' : '<';
        case event::button::KeyboardKeyId::Dot:
            return sp ? '.' : '>';
        case event::button::KeyboardKeyId::NumpadDot:
            return '.';
        case event::button::KeyboardKeyId::Slash:
            return sp ? '/' : '?';
        case event::button::KeyboardKeyId::NumpadSlash:
            return '/';
        case event::button::KeyboardKeyId::Backslash:
            return sp ? '\\' : '|';
        case event::button::KeyboardKeyId::Num0:
            return sp ? '0' : ')';
        case event::button::KeyboardKeyId::Numpad0:
            return '0';
        case event::button::KeyboardKeyId::Num1:
            return sp ? '1' : '!';
        case event::button::KeyboardKeyId::Numpad1:
            return '1';
        case event::button::KeyboardKeyId::Num2:
            return sp ? '2' : '@';
        case event::button::KeyboardKeyId::Numpad2:
            return '2';
        case event::button::KeyboardKeyId::Num3:
            return sp ? '3' : '#';
        case event::button::KeyboardKeyId::Numpad3:
            return '3';
        case event::button::KeyboardKeyId::Num4:
            return sp ? '4' : '$';
        case event::button::KeyboardKeyId::Numpad4:
            return '4';
        case event::button::KeyboardKeyId::Num5:
            return sp ? '5' : '%';
        case event::button::KeyboardKeyId::Numpad5:
            return '5';
        case event::button::KeyboardKeyId::Num6:
            return sp ? '6' : '^';
        case event::button::KeyboardKeyId::Numpad6:
            return '6';
        case event::button::KeyboardKeyId::Num7:
            return sp ? '7' : '&';
        case event::button::KeyboardKeyId::Numpad7:
            return '7';
        case event::button::KeyboardKeyId::Num8:
            return sp ? '8' : '*';
        case event::button::KeyboardKeyId::Numpad8:
            return '8';
        case event::button::KeyboardKeyId::Num9:
            return sp ? '9' : '(';
        case event::button::KeyboardKeyId::Numpad9:
            return '9';
        case event::button::KeyboardKeyId::NumpadStar:
            return '*';
        case event::button::KeyboardKeyId::NumpadPlus:
            return '+';
        case event::button::KeyboardKeyId::A:
            return sp ? 'a' : 'A';
        case event::button::KeyboardKeyId::B:
            return sp ? 'b' : 'B';
        case event::button::KeyboardKeyId::C:
            return sp ? 'c' : 'C';
        case event::button::KeyboardKeyId::D:
            return sp ? 'd' : 'D';
        case event::button::KeyboardKeyId::E:
            return sp ? 'e' : 'E';
        case event::button::KeyboardKeyId::F:
            return sp ? 'f' : 'F';
        case event::button::KeyboardKeyId::G:
            return sp ? 'g' : 'G';
        case event::button::KeyboardKeyId::H:
            return sp ? 'h' : 'H';
        case event::button::KeyboardKeyId::I:
            return sp ? 'i' : 'I';
        case event::button::KeyboardKeyId::J:
            return sp ? 'j' : 'J';
        case event::button::KeyboardKeyId::K:
            return sp ? 'k' : 'K';
        case event::button::KeyboardKeyId::L:
            return sp ? 'l' : 'L';
        case event::button::KeyboardKeyId::M:
            return sp ? 'm' : 'M';
        case event::button::KeyboardKeyId::N:
            return sp ? 'n' : 'N';
        case event::button::KeyboardKeyId::O:
            return sp ? 'o' : 'O';
        case event::button::KeyboardKeyId::P:
            return sp ? 'p' : 'P';
        case event::button::KeyboardKeyId::Q:
            return sp ? 'q' : 'Q';
        case event::button::KeyboardKeyId::R:
            return sp ? 'r' : 'R';
        case event::button::KeyboardKeyId::S:
            return sp ? 's' : 'S';
        case event::button::KeyboardKeyId::T:
            return sp ? 't' : 'T';
        case event::button::KeyboardKeyId::U:
            return sp ? 'u' : 'U';
        case event::button::KeyboardKeyId::V:
            return sp ? 'v' : 'V';
        case event::button::KeyboardKeyId::W:
            return sp ? 'w' : 'W';
        case event::button::KeyboardKeyId::X:
            return sp ? 'x' : 'X';
        case event::button::KeyboardKeyId::Y:
            return sp ? 'y' : 'Y';
        case event::button::KeyboardKeyId::Z:
            return sp ? 'z' : 'Z';
        default:
            return std::nullopt;
        }
    }
    return std::nullopt;
}
