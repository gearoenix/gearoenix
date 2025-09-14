#include "gx-plt-file-chooser.hpp"
#include "wasm/gx-plt-wasm-file-chooser.hpp"

#include <mutex>
#include <queue>

namespace {
struct OpenDialogData final {
    std::function<void(std::string&&, std::vector<char>&&)> on_open;
    std::function<void()> on_cancel;
    std::string title;
    std::string filter;
};

std::mutex open_dialogs_lock;
std::queue<OpenDialogData> open_dialogs;

#if GX_PLATFORM_WEBASSEMBLY
void open_callback(const std::string& name, const std::string&, const std::string_view buffer, void* const)
{
    const std::lock_guard _l(open_dialogs_lock);
    if (buffer.empty()) {
        open_dialogs.front().on_open(std::string(name), { buffer.begin(), buffer.end() });
    } else {
        open_dialogs.front().on_cancel();
    }
    open_dialogs.pop();
}
#else
struct SaveDialogData final {
    std::string name;
    std::string title;
    std::string filter;
    std::vector<char> content;
};

std::mutex save_dialogs_lock;
std::vector<SaveDialogData> save_dialogs;
#endif
}

void gearoenix::platform::file_chooser_open(std::function<void(std::string&&, std::vector<char>&&)>&& on_open, std::function<void()>&& on_cancel, std::string&& title, std::string&& filter)
{
    const std::lock_guard _l(open_dialogs_lock);
    open_dialogs.push({ std::move(on_open), std::move(on_cancel), std::move(title), std::move(filter) });
}

void gearoenix::platform::file_chooser_save(std::string&& name, std::string&& title, std::string&& filter, std::vector<char>&& content)
{
#if GX_PLATFORM_WEBASSEMBLY
    wasm::save(name, filter, { content.data(), content.size() });
#else
    const std::lock_guard _l(save_dialogs_lock);
    save_dialogs.emplace_back(std::move(name), std::move(title), std::move(filter), std::move(content));
#endif
}

void gearoenix::platform::file_chooser_update()
{
#if GX_PLATFORM_WEBASSEMBLY
    if (!open_dialogs.empty()) {
        const std::lock_guard _l(open_dialogs_lock);
        const auto& data = open_dialogs.front();
        wasm::open(data.filter, open_callback, nullptr);
    }
#else
#endif
}