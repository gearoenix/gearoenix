#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace gearoenix::platform::stream {
struct Path;
struct Stream;
}

namespace gearoenix::platform {
/// Open a system / WASM file picker, then deliver the chosen file to `on_open`. Two important
/// guarantees about the callback arguments:
///
/// - The `Stream` is the **only sandbox-safe way** to read the file's content. On WASM the
///   browser hands us an in-memory blob -- the `Path`'s raw string is not openable by the app
///   (the host filesystem is unreachable from JS-land). Always read content via the stream;
///   never `Stream::open(path)` it back. On native platforms the stream wraps the same file the
///   path identifies, so both are equivalent there -- but writing the WASM-safe code path costs
///   nothing and keeps the engine portable.
///
/// - The `Path` is for **display / cache-key** purposes only. Show it in the UI, use it as a
///   string key for caches, but don't treat it as a "go re-open this later" handle on any
///   platform that sandboxes filesystem access.
///
/// `on_cancel` fires if the user dismisses the picker without picking a file.
void file_chooser_open(std::function<void(stream::Path&&, std::shared_ptr<stream::Stream>&&)>&& on_open, std::function<void()>&& on_cancel, std::string&& title, std::string&& filter);
void file_chooser_save(std::string&& name, std::string&& title, std::string&& filter, std::vector<std::uint8_t>&& content, std::function<void()>&& on_cancel);
void file_chooser_update();
}