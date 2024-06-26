#ifndef GEAROENIX_CORE_STRING_HPP
#define GEAROENIX_CORE_STRING_HPP

#include "../platform/gx-plt-key.hpp"
#include "event/gx-cr-ev-button.hpp"
#include "gx-cr-language.hpp"
#include <optional>
#include <string>

#ifdef GX_PLATFORM_IOS
#import <Foundation/Foundation.h>
#endif

namespace gearoenix::core {
struct String {
public:
    [[nodiscard]] static bool is_character(platform::key::Id);
    [[nodiscard]] static std::optional<wchar_t> to_character(platform::key::Id, bool shift_pressed = false, Language language = Language::English);
    [[nodiscard]] static std::string to_string(const std::wstring& s);
    [[nodiscard]] static std::wstring to_wstring(const std::string& s);
    [[nodiscard]] static const wchar_t* to_wchar_ptr(const std::string& s);

#ifdef GX_IN_IOS
    [[nodiscard]] static NSString* to_objc_string(const std::string& s);
    [[nodiscard]] static NSString* to_objc_string(const char* s);
    [[nodiscard]] static std::string join_path(const NSString* dir, const std::string& s);
    [[nodiscard]] static std::string join_path(const NSString* dir, const char* s);
    [[nodiscard]] static std::string join_path(const NSString* dir, const NSString* s);
#endif
};
}
#endif
