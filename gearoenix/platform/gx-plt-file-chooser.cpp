#include "gx-plt-file-chooser.hpp"
#include "../core/macro/gx-cr-mcr-assert.hpp"
#include "gx-plt-log.hpp"
#include "stream/gx-plt-stm-path.hpp"
#include "stream/gx-plt-stm-stream.hpp"

#if GX_PLATFORM_WEBASSEMBLY
#include "wasm/gx-plt-wasm-file-chooser.hpp"
#else
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#endif

#include <mutex>
#include <queue>

namespace {
struct OpenDialogData final {
    std::function<void(gearoenix::platform::stream::Path&&, std::shared_ptr<gearoenix::platform::stream::Stream>&&)> on_open;
    std::function<void()> on_cancel;
    std::string title;
    std::string filter;
};

struct SaveDialogData final {
    std::string name;
    std::string title;
    std::string filter;
    std::vector<char> content;
};

std::mutex dialogs_lock;
bool dialogs_running = false;
std::queue<std::variant<OpenDialogData, SaveDialogData>> dialogs;

const std::string file_browser_key = "gearoenix_platform_file_chooser_key";

#if GX_PLATFORM_WEBASSEMBLY
void open_callback(const std::string& name, const std::string&, const std::string_view buffer, void* const)
{
    const std::lock_guard _l(open_dialogs_lock);
    if (!buffer.empty()) {
        open_dialogs.front().on_open(std::string(name), { buffer.begin(), buffer.end() });
    } else {
        open_dialogs.front().on_cancel();
    }
    open_dialogs.pop();
    open_dialogs_running = false;
}
#endif
}

void gearoenix::platform::file_chooser_open(std::function<void(stream::Path&&, std::shared_ptr<stream::Stream>&&)>&& on_open, std::function<void()>&& on_cancel, std::string&& title, std::string&& filter)
{
    const std::lock_guard _l(dialogs_lock);
    dialogs.emplace(OpenDialogData { std::move(on_open), std::move(on_cancel), std::move(title), std::move(filter) });
}

void gearoenix::platform::file_chooser_save(std::string&& name, std::string&& title, std::string&& filter, std::vector<char>&& content)
{
#if GX_PLATFORM_WEBASSEMBLY
    wasm::save(name, filter, { content.data(), content.size() });
#else
    const std::lock_guard _l(dialogs_lock);
    dialogs.emplace(SaveDialogData { std::move(name), std::move(title), std::move(filter), std::move(content) });
#endif
}

void gearoenix::platform::file_chooser_update()
{
    if (!dialogs.empty() && !dialogs_running) {
        const std::lock_guard _l(dialogs_lock);
        dialogs_running = true;
        const auto& data = dialogs.front();
        if (std::holds_alternative<OpenDialogData>(data)) {
            const auto& d = std::get<OpenDialogData>(data);
#if GX_PLATFORM_WEBASSEMBLY
            wasm::open(d.filter, open_callback, nullptr);
#else
            ImGuiFileDialog::Instance()->OpenDialog(file_browser_key, d.title, d.filter.c_str());
        } else {
            const auto& d = std::get<SaveDialogData>(data);
            ImGuiFileDialog::Instance()->OpenDialog(file_browser_key, d.title, d.filter.c_str());
        }
#endif
    }

    if (ImGuiFileDialog::Instance()->Display(open_file_browser_key)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            auto name = ImGuiFileDialog::Instance()->GetFilePathName();
            auto path = stream::Path::create_absolute(std::move(name));
            auto stream = stream::Stream::open(path);
            const std::lock_guard _l(open_dialogs_lock);
            open_dialogs.front().on_open(std::move(path), std::move(stream));
            open_dialogs.pop();
            open_dialogs_running = false;
        }
        ImGuiFileDialog::Instance()->Close();
    }
    if (!ImGuiFileDialog::Instance()->IsOpened() && open_dialogs_running) {
        const std::lock_guard _l(open_dialogs_lock);
        open_dialogs.front().on_cancel();
        open_dialogs.pop();
        open_dialogs_running = false;
    }
}