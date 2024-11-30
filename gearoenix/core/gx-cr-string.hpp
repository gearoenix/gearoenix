#pragma once
#include "../platform/gx-plt-key.hpp"
#include "gx-cr-language.hpp"
#include <array>
#include <boost/core/demangle.hpp>
#include <cstdint>
#include <optional>
#include <string>
#include <typeinfo>

#ifdef GX_PLATFORM_IOS
#import <Foundation/Foundation.h>
#endif

namespace gearoenix::core {
struct String final {
    [[nodiscard]] static bool is_character(platform::key::Id);
    [[nodiscard]] static std::optional<wchar_t> to_character(platform::key::Id, bool shift_pressed = false, Language language = Language::English);
    [[nodiscard]] static std::string to_string(const std::wstring& s);
    [[nodiscard]] static std::wstring to_wstring(const std::string& s);
    [[nodiscard]] static const wchar_t* to_wchar_ptr(const std::string& s);

    template <typename T>
    [[nodiscard]] constexpr static auto to_hex_string(const T val)
    {
        constexpr auto hex_map = "0123456789ABCDEF";
        std::array<char, sizeof(T) * 2 + 3> hex_string;
        hex_string[0] = '0';
        hex_string[1] = 'X';
        for (std::uint32_t i = 0; i < sizeof(T) * 2; ++i) {
            hex_string[sizeof(T) * 2 + 1 - i] = hex_map[(val >> static_cast<T>(i << 2)) & static_cast<T>(0xF)];
        }

        hex_string[sizeof(T) * 2 + 2] = '\0';
        return hex_string;
    }

    template <typename T>
    [[nodiscard]] constexpr static auto ptr_to_hex_string(const T* const ptr)
    {
        return to_hex_string(reinterpret_cast<std::uintptr_t>(ptr));
    }

    template <typename T>
    [[nodiscard]] static auto type_name()
    {
        return boost::core::demangle(typeid(std::remove_cv_t<T>).name());
    }

    template <typename T>
    [[nodiscard]] static auto type_name(const T* const)
    {
        return type_name<T>();
    }

    template <typename T>
    [[nodiscard]] static const std::string& ptr_name(const T* const ptr)
    {
        static const auto tn = type_name(ptr);
        const auto ph = ptr_to_hex_string(ptr);
        thread_local std::string tpn;
        tpn.clear();
        tpn += tn;
        tpn += " [";
        tpn += ph.data();
        tpn += "]";
        return tpn;
    }

#ifdef GX_IN_IOS
    [[nodiscard]] static NSString* to_objc_string(const std::string& s);
    [[nodiscard]] static NSString* to_objc_string(const char* s);
    [[nodiscard]] static std::string join_path(const NSString* dir, const std::string& s);
    [[nodiscard]] static std::string join_path(const NSString* dir, const char* s);
    [[nodiscard]] static std::string join_path(const NSString* dir, const NSString* s);
#endif
};
}