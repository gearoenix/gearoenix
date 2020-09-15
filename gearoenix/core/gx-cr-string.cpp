#include "gx-cr-string.hpp"

#include <codecvt>
#include <locale>

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
            return sp ? '~' : '`';
        case event::button::KeyboardKeyId::Less:
            return '<';
        case event::button::KeyboardKeyId::Greater:
            return '>';
        case event::button::KeyboardKeyId::Tab:
            return '\t';
        case event::button::KeyboardKeyId::Space:
            return ' ';
        case event::button::KeyboardKeyId::Minus:
            return sp ? '_' : '-';
        case event::button::KeyboardKeyId::NumpadMinus:
            return '-';
        case event::button::KeyboardKeyId::Equal:
            return sp ? '+' : '=';
        case event::button::KeyboardKeyId::LeftBracket:
            return sp ? '{' : '[';
        case event::button::KeyboardKeyId::RightBracket:
            return sp ? '}' : ']';
        case event::button::KeyboardKeyId::Semicolon:
            return sp ? ':' : ';';
        case event::button::KeyboardKeyId::Quote:
            return sp ? '"' : '\'';
        case event::button::KeyboardKeyId::Comma:
            return sp ? '<' : ',';
        case event::button::KeyboardKeyId::Dot:
            return sp ? '>' : '.';
        case event::button::KeyboardKeyId::NumpadDot:
            return '.';
        case event::button::KeyboardKeyId::Slash:
            return sp ? '?' : '/';
        case event::button::KeyboardKeyId::NumpadSlash:
            return '/';
        case event::button::KeyboardKeyId::Backslash:
            return sp ? '|' : '\\';
        case event::button::KeyboardKeyId::Num0:
            return sp ? ')' : '0';
        case event::button::KeyboardKeyId::Numpad0:
            return '0';
        case event::button::KeyboardKeyId::Num1:
            return sp ? '!' : '1';
        case event::button::KeyboardKeyId::Numpad1:
            return '1';
        case event::button::KeyboardKeyId::Num2:
            return sp ? '@' : '2';
        case event::button::KeyboardKeyId::Numpad2:
            return '2';
        case event::button::KeyboardKeyId::Num3:
            return sp ? '#' : '3';
        case event::button::KeyboardKeyId::Numpad3:
            return '3';
        case event::button::KeyboardKeyId::Num4:
            return sp ? '$' : '4';
        case event::button::KeyboardKeyId::Numpad4:
            return '4';
        case event::button::KeyboardKeyId::Num5:
            return sp ? '%' : '5';
        case event::button::KeyboardKeyId::Numpad5:
            return '5';
        case event::button::KeyboardKeyId::Num6:
            return sp ? '^' : '6';
        case event::button::KeyboardKeyId::Numpad6:
            return '6';
        case event::button::KeyboardKeyId::Num7:
            return sp ? '&' : '7';
        case event::button::KeyboardKeyId::Numpad7:
            return '7';
        case event::button::KeyboardKeyId::Num8:
            return sp ? '*' : '8';
        case event::button::KeyboardKeyId::Numpad8:
            return '8';
        case event::button::KeyboardKeyId::Num9:
            return sp ? '(' : '9';
        case event::button::KeyboardKeyId::Numpad9:
            return '9';
        case event::button::KeyboardKeyId::NumpadStar:
            return '*';
        case event::button::KeyboardKeyId::NumpadPlus:
            return '+';
        case event::button::KeyboardKeyId::A:
            return sp ? 'A' : 'a';
        case event::button::KeyboardKeyId::B:
            return sp ? 'B' : 'b';
        case event::button::KeyboardKeyId::C:
            return sp ? 'C' : 'c';
        case event::button::KeyboardKeyId::D:
            return sp ? 'D' : 'd';
        case event::button::KeyboardKeyId::E:
            return sp ? 'E' : 'e';
        case event::button::KeyboardKeyId::F:
            return sp ? 'F' : 'f';
        case event::button::KeyboardKeyId::G:
            return sp ? 'G' : 'g';
        case event::button::KeyboardKeyId::H:
            return sp ? 'H' : 'h';
        case event::button::KeyboardKeyId::I:
            return sp ? 'I' : 'i';
        case event::button::KeyboardKeyId::J:
            return sp ? 'J' : 'j';
        case event::button::KeyboardKeyId::K:
            return sp ? 'K' : 'k';
        case event::button::KeyboardKeyId::L:
            return sp ? 'L' : 'l';
        case event::button::KeyboardKeyId::M:
            return sp ? 'M' : 'm';
        case event::button::KeyboardKeyId::N:
            return sp ? 'N' : 'n';
        case event::button::KeyboardKeyId::O:
            return sp ? 'O' : 'o';
        case event::button::KeyboardKeyId::P:
            return sp ? 'P' : 'p';
        case event::button::KeyboardKeyId::Q:
            return sp ? 'Q' : 'q';
        case event::button::KeyboardKeyId::R:
            return sp ? 'R' : 'r';
        case event::button::KeyboardKeyId::S:
            return sp ? 'S' : 's';
        case event::button::KeyboardKeyId::T:
            return sp ? 'T' : 't';
        case event::button::KeyboardKeyId::U:
            return sp ? 'U' : 'u';
        case event::button::KeyboardKeyId::V:
            return sp ? 'V' : 'v';
        case event::button::KeyboardKeyId::W:
            return sp ? 'W' : 'w';
        case event::button::KeyboardKeyId::X:
            return sp ? 'X' : 'x';
        case event::button::KeyboardKeyId::Y:
            return sp ? 'Y' : 'y';
        case event::button::KeyboardKeyId::Z:
            return sp ? 'Z' : 'z';
        default:
            return std::nullopt;
        }
    }
    return std::nullopt;
}

std::string gearoenix::core::String::to_string(const std::wstring& s) noexcept
{
    return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(s);
}

#ifdef GX_IN_IOS
NSString* gearoenix::core::String::to_objc_string(const std::string& s) noexcept
{
    return to_objc_string(s.c_str());
}

NSString* gearoenix::core::String::to_objc_string(const char* const s) noexcept
{
    return [NSString stringWithCString:s encoding:[NSString defaultCStringEncoding]];
}

std::string gearoenix::core::String::join_path(const NSString* dir, const std::string& s) noexcept
{
    return join_path(dir, s.c_str());
}

std::string gearoenix::core::String::join_path(const NSString* dir, const char* s) noexcept
{
    return join_path(dir, to_objc_string(s));
}

std::string gearoenix::core::String::join_path(const NSString* dir, const NSString* s) noexcept
{
    NSArray* parts = [NSArray arrayWithObjects:dir, s, (NSString*)nil];
    return std::string([[NSString pathWithComponents:parts] fileSystemRepresentation]);
}

#endif
