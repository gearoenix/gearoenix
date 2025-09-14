#pragma once
#include "../gx-plt-build-configuration.hpp"
#if GX_PLATFORM_WEBASSEMBLY
#include <string>
#include <string_view>

namespace gearoenix::platform::wasm {
using open_callback = void (*)(const std::string& name, const std::string& type, std::string_view buffer, void* callback_data);

void open(const std::string& accept_types, open_callback callback, void* callback_data);
void save(const std::string& file_name, const std::string& mime_type, std::string_view buffer);
}
#endif