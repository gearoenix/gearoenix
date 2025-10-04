#include "gx-plt-file-chooser.hpp"
#include "../core/macro/gx-cr-mcr-assert.hpp"
#include "gx-plt-log.hpp"
#include "stream/gx-plt-stm-memory.hpp"
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
    std::vector<std::uint8_t> content;
    std::function<void()> on_cancel;
};

std::mutex dialogs_lock;
bool dialogs_running = false;
std::queue<std::variant<OpenDialogData, SaveDialogData>> dialogs;

const std::string file_browser_key = "gearoenix_platform_file_chooser_key";

#if GX_PLATFORM_WEBASSEMBLY
void open_callback(std::string&& name, const std::string&, std::vector<std::uint8_t>&& buffer, void* const)
{
    const std::lock_guard _l(dialogs_lock);
    GX_ASSERT_D(!dialogs.empty());
    GX_ASSERT_D(dialogs_running);
    GX_ASSERT_D(std::holds_alternative<OpenDialogData>(dialogs.front()));
    std::get<OpenDialogData>(dialogs.front()).on_open(gearoenix::platform::stream::Path::create_absolute(std::move(name)), std::make_shared<gearoenix::platform::stream::Memory>(std::move(buffer)));
    dialogs.pop();
    dialogs_running = false;
}
#endif
}

void gearoenix::platform::file_chooser_open(std::function<void(stream::Path&&, std::shared_ptr<stream::Stream>&&)>&& on_open, std::function<void()>&& on_cancel, std::string&& title, std::string&& filter)
{
    const std::lock_guard _l(dialogs_lock);
    dialogs.emplace(OpenDialogData { std::move(on_open), std::move(on_cancel), std::move(title), std::move(filter) });
}

void gearoenix::platform::file_chooser_save(std::string&& name, std::string&& title, std::string&& filter, std::vector<std::uint8_t>&& content, std::function<void()>&& on_cancel)
{
#if GX_PLATFORM_WEBASSEMBLY
    wasm::save(name, filter, std::move(content));
#else
    const std::lock_guard _l(dialogs_lock);
    dialogs.emplace(SaveDialogData { std::move(name), std::move(title), std::move(filter), std::move(content), std::move(on_cancel) });
#endif
}

void gearoenix::platform::file_chooser_update()
{
    if (!dialogs.empty() && !dialogs_running) {
        const std::lock_guard _l(dialogs_lock);
        dialogs_running = true;
        auto& data = dialogs.front();
        if (std::holds_alternative<OpenDialogData>(data)) {
            const auto& d = std::get<OpenDialogData>(data);
#if GX_PLATFORM_WEBASSEMBLY
            wasm::open(d.filter, open_callback, nullptr);
#else
            ImGuiFileDialog::Instance()->OpenDialog(file_browser_key, d.title, d.filter.c_str());
#endif
        } else {
            auto& d = std::get<SaveDialogData>(data);
#if GX_PLATFORM_WEBASSEMBLY
            wasm::save(d.name, d.filter, std::move(d.content));
            dialogs_running = false;
#else
            ImGuiFileDialog::Instance()->OpenDialog(file_browser_key, d.title, d.filter.c_str());
#endif
        }
    }

#if GX_PLATFORM_WEBASSEMBLY
#else
    if (ImGuiFileDialog::Instance()->Display(file_browser_key)) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            auto name = ImGuiFileDialog::Instance()->GetFilePathName();
            auto path = stream::Path::create_absolute(std::move(name));
            auto stream = stream::Stream::open(path);
            const auto data = [] {
                const std::lock_guard _(dialogs_lock);
                auto d = std::move(dialogs.front());
                dialogs.pop();
                return d;
            }();
            if (std::holds_alternative<OpenDialogData>(data)) {
                const auto& d = std::get<OpenDialogData>(data);
                d.on_open(std::move(path), std::move(stream));
            } else {
                const auto& d = std::get<SaveDialogData>(data);
                stream->write(d.content);
            }
            dialogs_running = false;
        }
        ImGuiFileDialog::Instance()->Close();
    }
    if (!ImGuiFileDialog::Instance()->IsOpened() && dialogs_running) {
        const auto data = [] {
            const std::lock_guard _(dialogs_lock);
            auto d = std::move(dialogs.front());
            dialogs.pop();
            return d;
        }();
        if (std::holds_alternative<OpenDialogData>(data)) {
            const auto& d = std::get<OpenDialogData>(data);
            d.on_cancel();
        } else {
            const auto& d = std::get<SaveDialogData>(data);
            d.on_cancel();
        }
        dialogs_running = false;
    }
#endif
}