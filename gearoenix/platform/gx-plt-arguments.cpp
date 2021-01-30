#include "gx-plt-arguments.hpp"
#include "../core/gx-cr-string.hpp"

std::string gearoenix::platform::Arguments::extract_process_directory(const std::string& s) noexcept
{
#ifdef GX_PLATFORM_DESKTOP
#ifdef GX_PLATFORM_WINDOWS
#define GX_DIR_SEP '\\'
#else
#define GX_DIR_SEP '/'
#endif
    for (std::size_t i = s.size() - 1; i > 0; --i)
        if (s[i] == GX_DIR_SEP)
            return s.substr(0, i + 1);
    return std::string("");
#else
    (void)s;
    return std::string("");
#endif
}

gearoenix::platform::Arguments::Arguments(GX_MAIN_ENTRY_ARGS_DEF) noexcept
{
#ifdef GX_PLATFORM_INTERFACE_WIN32
    int argc = 0;
    auto* const w_argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (nullptr == w_argv) {
        GX_LOG_F("Can not fetch arguments.")
    }
    std::vector<std::string> s_argv(argc);
    std::vector<const char*> argv(argc);
    for (int i = 0; i < argc; ++i) {
        s_argv[i] = core::String::to_string(std::wstring(w_argv[i]));
        argv[i] = s_argv[i].c_str();
    }
    LocalFree(w_argv);
#endif

    process_name = argv[0];
    process_directory = extract_process_directory(process_name);
    tokens.resize(argc - 1);
    has_tokens = argc > 1;

    GX_LOG_D("Parsing arguments")
    for (int ai = 1, ti = 0; ai < argc; ++ai, ++ti) {
        GX_LOG_D("Application Arg[" << ti << "] = " << argv[ai])
        tokens[ti] = std::string(argv[ai]);
    }

    for (std::size_t ti = 0; ti < tokens.size();) {
        const auto& token = tokens[ti];
        if (!is_key(token)) {
            GX_LOG_F("Unexpected token '" << token << "'.")
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
}

bool gearoenix::platform::Arguments::is_key(const std::string& s) noexcept
{
    return s[0] == '-' && s[1] == '-';
}

bool gearoenix::platform::Arguments::get_value(const std::string& key, std::string& value, bool necessary) const noexcept
{
    auto range = map.equal_range(key);
    int count = 0;
    for (auto i = range.first; i != range.second; ++i, ++count) {
        if (i->second.has_value()) {
            value = i->second.value();
        } else {
            GX_LOG_F("The '" << key << "' key must have value.")
        }
    }
    if (count < 1 && necessary) {
        GX_LOG_F("The '" << key << "' key is needed and must have value.")
    } else if (count > 1) {
        GX_LOG_F("The '" << key << "' key is needed only once.")
    }
    return false;
}

bool gearoenix::platform::Arguments::get_value(const std::string& key, int& value, bool necessary) const noexcept
{
    std::string v;
    if (!get_value(key, v, necessary))
        return false;
    value = std::stoi(v);
    return true;
}
