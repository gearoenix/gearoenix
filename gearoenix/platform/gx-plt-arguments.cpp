#include "gx-plt-arguments.hpp"
#include "../core/gx-cr-string.hpp"
#include "gx-plt-log.hpp"

namespace {
[[nodiscard]] std::string extract_process_directory(const std::string& s)
{
#ifdef GX_PLATFORM_DESKTOP
#ifdef GX_PLATFORM_WINDOWS
#define GX_DIR_SEP '\\'
#else
#define GX_DIR_SEP '/'
#endif
    for (auto i = s.size() - 1; i > 0; --i) {
        if (s[i] == GX_DIR_SEP) {
            return s.substr(0, i + 1);
        }
    }
    return std::string("");
#else
    (void)s;
    return std::string("");
#endif
}

[[nodiscard]] bool is_key(const std::string& s)
{
    return s.size() > 2 && s[0] == '-' && s[1] == '-';
}

gearoenix::platform::Arguments instance;
}

gearoenix::platform::Arguments::Arguments() = default;

gearoenix::platform::Arguments& gearoenix::platform::Arguments::get()
{
    return instance;
}

void gearoenix::platform::Arguments::parse(const int argc, const char* const* const argv)
{
    process_name = argv[0];
    process_directory = extract_process_directory(process_name);
    tokens.resize(argc - 1);
    has_tokens = argc > 1;

    GX_LOG_D("Parsing arguments");
    for (int ai = 1, ti = 0; ai < argc; ++ai, ++ti) {
        GX_LOG_D("Application Arg[" << ti << "] = " << argv[ai]);
        tokens[ti] = std::string(argv[ai]);
    }

    for (std::uint32_t ti = 0; ti < tokens.size();) {
        const auto& token = tokens[ti];
        if (!is_key(token)) {
            GX_LOG_F("Unexpected token '" << token << "'.");
        }
        const auto key = token.substr(2);
        ++ti;
        if (ti >= tokens.size()) {
            map.insert(std::make_pair(key, std::nullopt));
            break;
        }
        const auto& next_token = tokens[ti];
        if (is_key(next_token)) {
            map.insert(std::make_pair(key, std::nullopt));
            continue;
        }
        map.insert(std::make_pair(key, next_token));
        ++ti;
    }
    GX_LOG_D("Parsing of arguments has been finished.");
}

bool gearoenix::platform::Arguments::get_value(const std::string& key, std::string& value, const bool necessary) const
{
    const auto range = map.equal_range(key);
    int count = 0;
    for (auto i = range.first; i != range.second; ++i, ++count) {
        if (i->second.has_value()) {
            value = i->second.value();
        } else {
            GX_LOG_F("The '" << key << "' key must have value.");
        }
    }
    if (count < 1 && necessary) {
        GX_LOG_F("The '" << key << "' key is needed and must have value.");
    }
    if (count > 1) {
        GX_LOG_F("The '" << key << "' key is needed only once.");
    }
    return false;
}

bool gearoenix::platform::Arguments::get_value(const std::string& key, int& value, const bool necessary) const
{
    std::string v;
    if (!get_value(key, v, necessary)) {
        return false;
    }
    value = std::stoi(v);
    return true;
}
