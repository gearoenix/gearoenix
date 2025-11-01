#pragma once
#include "../core/macro/gx-cr-mcr-getter-setter.hpp"

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace gearoenix::platform {
struct Arguments {
    typedef std::multimap<std::string, std::optional<std::string>> Map;

    GX_GET_CREF_PRV(std::string, process_name);
    GX_GET_CREF_PRV(std::string, process_directory);
    GX_GET_CREF_PRV(std::vector<std::string>, tokens);
    GX_GET_CREF_PRV(Map, map);
    GX_GET_VAL_PRV(bool, has_tokens, false);

public:
    Arguments();
    [[nodiscard]] static Arguments& get();
    void parse(int argc, const char* const* argv);
    [[nodiscard]] bool get_value(const std::string& key, std::string& value, bool necessary = false) const;
    [[nodiscard]] bool get_value(const std::string& key, int& value, bool necessary = false) const;
};
}