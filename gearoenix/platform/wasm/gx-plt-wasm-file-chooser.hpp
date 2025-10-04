#pragma once
#include "../gx-plt-build-configuration.hpp"
#if GX_PLATFORM_WEBASSEMBLY
#include <string>
#include <vector>

namespace gearoenix::platform::wasm {
using open_callback = void (*)(std::string&& name, const std::string& type, std::vector<std::uint8_t>&& buffer, void* callback_data);

void open(const std::string& accept_types, open_callback callback, void* callback_data);
void save(const std::string& file_name, const std::string& mime_type, std::vector<std::uint8_t>&& buffer);
}
#endif