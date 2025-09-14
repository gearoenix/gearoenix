#include "gx-plt-wasm-file-chooser.hpp"
#if GX_PLATFORM_WEBASSEMBLY

#include "../gx-plt-log.hpp"

#include <emscripten.h>

// clang-format off
EM_JS(void, upload, (char const* accept_types, gearoenix::platform::wasm::open_callback callback, void* callback_data), {
    globalThis["open_file"] = function(e)
    {
        const file_reader = new FileReader();
        file_reader.onload = (event) =>
        {
            const uint8Arr = new Uint8Array(event.target.result);
            const data_ptr = Module["_malloc"](uint8Arr.length);
            const data_on_heap = new Uint8Array(Module["HEAPU8"].buffer, data_ptr, uint8Arr.length);
            data_on_heap.set(uint8Arr);
            Module["ccall"]('gearoenix_upload_file_return', 'number', [ 'string', 'string', 'number', 'number', 'number', 'number' ], [event.target.filename, event.target.mime_type, data_on_heap.byteOffset, uint8Arr.length, callback, callback_data]);
            Module["_free"](data_ptr);
        };
        file_reader.filename = e.target.files[0].name;
        file_reader.mime_type = e.target.files[0].type;
        file_reader.readAsArrayBuffer(e.target.files[0]);
    };
    var file_selector = document.createElement('input');
    file_selector.setAttribute('type', 'file');
    file_selector.setAttribute('onchange', 'globalThis["open_file"](event)');
    file_selector.addEventListener('cancel', () => { Module["ccall"]('gearoenix_upload_file_return', 'number', ['string', 'string', 'number', 'number', 'number', 'number'], ["", "", 0, 0, callback, callback_data]); });
    file_selector.setAttribute('accept', UTF8ToString(accept_types));
    var is_safari = /^((?!chrome|android).)*safari/i.test(navigator.userAgent);
    if (is_safari) {
        var dialog = document.createElement('dialog');
        dialog.setAttribute('id', 'EmJsFileDialog');
        var desc = document.createElement('p');
        desc.innerText = 'Please choose a file. Allowed extension(s): ' + UTF8ToString(accept_types);
        dialog.appendChild(desc);
        file_selector.setAttribute('onclick', 'var dg = document.getElementById("EmJsFileDialog"); dg.close(); dg.remove()');
        dialog.appendChild(file_selector);
        document.body.append(dialog);
        dialog.showModal();
    } else {
        file_selector.click();
    }
});

EM_JS(void, download, (char const* filename, char const* mime_type, void const* buffer, size_t buffer_size), {
    var a = document.createElement('a');
    a.download = UTF8ToString(filename);
    var start = buffer >>> 0;
    var end = (start + (buffer_size >>> 0)) >>> 0;
    var view = Module["HEAPU8"].subarray(start, end);
    var copy = view.slice();
    a.href = URL.createObjectURL(new Blob([copy], { type: UTF8ToString(mime_type) }));
    a.click();
});
// clang-format on

// #pragma GCC diagnostic pop

extern "C" {
EMSCRIPTEN_KEEPALIVE int gearoenix_upload_file_return(char const* const filename, char const* const mime_type, const char* const buffer, const size_t buffer_size, const gearoenix::platform::wasm::open_callback callback, void* const callback_data)
{
    callback(filename, mime_type, !buffer || buffer_size == 0 ? std::string_view() : std::string_view { buffer, buffer_size }, callback_data);
    return 1;
}
}

void gearoenix::platform::wasm::open(const std::string& accept_types, const open_callback callback, void* const callback_data)
{
    ::upload(accept_types.c_str(), callback, callback_data);
}

void gearoenix::platform::wasm::save(const std::string& file_name, const std::string& mime_type, const std::string_view buffer)
{
    GX_LOG_D("download called");
    ::download(file_name.c_str(), mime_type.c_str(), buffer.data(), buffer.size());
}

#endif