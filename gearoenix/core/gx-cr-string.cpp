#include "gx-cr-string.hpp"
#include <codecvt>
#include <locale>

namespace {
thread_local std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
}

bool gearoenix::core::String::is_character(const platform::key::Id ki)
{
    return to_character(ki).has_value();
}

std::optional<wchar_t> gearoenix::core::String::to_character(const platform::key::Id ki, const bool sp, const Language l)
{
    if (l == Language::English) {
        switch (ki) {
        case platform::key::Id::Enter:
        case platform::key::Id::NumpadEnter:
            return '\n';
        case platform::key::Id::Tilda:
            return sp ? '~' : '`';
        case platform::key::Id::Less:
            return '<';
        case platform::key::Id::Greater:
            return '>';
        case platform::key::Id::Tab:
            return '\t';
        case platform::key::Id::Space:
            return ' ';
        case platform::key::Id::Minus:
            return sp ? '_' : '-';
        case platform::key::Id::NumpadMinus:
            return '-';
        case platform::key::Id::Equal:
            return sp ? '+' : '=';
        case platform::key::Id::LeftBracket:
            return sp ? '{' : '[';
        case platform::key::Id::RightBracket:
            return sp ? '}' : ']';
        case platform::key::Id::Semicolon:
            return sp ? ':' : ';';
        case platform::key::Id::Quote:
            return sp ? '"' : '\'';
        case platform::key::Id::Comma:
            return sp ? '<' : ',';
        case platform::key::Id::Dot:
            return sp ? '>' : '.';
        case platform::key::Id::NumpadDot:
            return '.';
        case platform::key::Id::Slash:
            return sp ? '?' : '/';
        case platform::key::Id::NumpadSlash:
            return '/';
        case platform::key::Id::Backslash:
            return sp ? '|' : '\\';
        case platform::key::Id::Num0:
            return sp ? ')' : '0';
        case platform::key::Id::Numpad0:
            return '0';
        case platform::key::Id::Num1:
            return sp ? '!' : '1';
        case platform::key::Id::Numpad1:
            return '1';
        case platform::key::Id::Num2:
            return sp ? '@' : '2';
        case platform::key::Id::Numpad2:
            return '2';
        case platform::key::Id::Num3:
            return sp ? '#' : '3';
        case platform::key::Id::Numpad3:
            return '3';
        case platform::key::Id::Num4:
            return sp ? '$' : '4';
        case platform::key::Id::Numpad4:
            return '4';
        case platform::key::Id::Num5:
            return sp ? '%' : '5';
        case platform::key::Id::Numpad5:
            return '5';
        case platform::key::Id::Num6:
            return sp ? '^' : '6';
        case platform::key::Id::Numpad6:
            return '6';
        case platform::key::Id::Num7:
            return sp ? '&' : '7';
        case platform::key::Id::Numpad7:
            return '7';
        case platform::key::Id::Num8:
            return sp ? '*' : '8';
        case platform::key::Id::Numpad8:
            return '8';
        case platform::key::Id::Num9:
            return sp ? '(' : '9';
        case platform::key::Id::Numpad9:
            return '9';
        case platform::key::Id::NumpadStar:
            return '*';
        case platform::key::Id::NumpadPlus:
            return '+';
        case platform::key::Id::A:
            return sp ? 'A' : 'a';
        case platform::key::Id::B:
            return sp ? 'B' : 'b';
        case platform::key::Id::C:
            return sp ? 'C' : 'c';
        case platform::key::Id::D:
            return sp ? 'D' : 'd';
        case platform::key::Id::E:
            return sp ? 'E' : 'e';
        case platform::key::Id::F:
            return sp ? 'F' : 'f';
        case platform::key::Id::G:
            return sp ? 'G' : 'g';
        case platform::key::Id::H:
            return sp ? 'H' : 'h';
        case platform::key::Id::I:
            return sp ? 'I' : 'i';
        case platform::key::Id::J:
            return sp ? 'J' : 'j';
        case platform::key::Id::K:
            return sp ? 'K' : 'k';
        case platform::key::Id::L:
            return sp ? 'L' : 'l';
        case platform::key::Id::M:
            return sp ? 'M' : 'm';
        case platform::key::Id::N:
            return sp ? 'N' : 'n';
        case platform::key::Id::O:
            return sp ? 'O' : 'o';
        case platform::key::Id::P:
            return sp ? 'P' : 'p';
        case platform::key::Id::Q:
            return sp ? 'Q' : 'q';
        case platform::key::Id::R:
            return sp ? 'R' : 'r';
        case platform::key::Id::S:
            return sp ? 'S' : 's';
        case platform::key::Id::T:
            return sp ? 'T' : 't';
        case platform::key::Id::U:
            return sp ? 'U' : 'u';
        case platform::key::Id::V:
            return sp ? 'V' : 'v';
        case platform::key::Id::W:
            return sp ? 'W' : 'w';
        case platform::key::Id::X:
            return sp ? 'X' : 'x';
        case platform::key::Id::Y:
            return sp ? 'Y' : 'y';
        case platform::key::Id::Z:
            return sp ? 'Z' : 'z';
        default:
            return std::nullopt;
        }
    }
    return std::nullopt;
}

std::string gearoenix::core::String::to_string(const std::wstring& s)
{
    return converter.to_bytes(s);
}

std::wstring gearoenix::core::String::to_wstring(const std::string& s)
{
    return converter.from_bytes(s);
}

const wchar_t* gearoenix::core::String::to_wchar_ptr(const std::string& s)
{
    static thread_local std::wstring w;
    w = to_wstring(s);
    return w.c_str();
}

#ifdef GX_IN_IOS
NSString* gearoenix::core::String::to_objc_string(const std::string& s)
{
    return to_objc_string(s.c_str());
}

NSString* gearoenix::core::String::to_objc_string(const char* const s)
{
    return [NSString stringWithCString:s encoding:[NSString defaultCStringEncoding]];
}

std::string gearoenix::core::String::join_path(const NSString* dir, const std::string& s)
{
    return join_path(dir, s.c_str());
}

std::string gearoenix::core::String::join_path(const NSString* dir, const char* s)
{
    return join_path(dir, to_objc_string(s));
}

std::string gearoenix::core::String::join_path(const NSString* dir, const NSString* s)
{
    NSArray* parts = [NSArray arrayWithObjects:dir, s, (NSString*)nil];
    return std::string([[NSString pathWithComponents:parts] fileSystemRepresentation]);
}

#endif
