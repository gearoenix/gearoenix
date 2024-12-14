#pragma once
#include "gx-plt-stm-stream.hpp"
#include <fstream>

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::platform::stream {
struct Local final : Stream {
private:
    std::fstream file;
    explicit Local(std::fstream file);

public:
    Local(const Application& app, const std::string& name, bool writable = false);
    ~Local() override;
    [[nodiscard]] static Local* open(const Application& app, const std::string& name, bool writable = false);
    [[nodiscard]] stream_size_t read(void* data, stream_size_t length) override;
    [[nodiscard]] stream_size_t write(const void* data, stream_size_t length) override;
    void seek(stream_size_t offset) override;
    [[nodiscard]] stream_size_t tell() override;
    [[nodiscard]] stream_size_t size() override;
    void flush() override;
    [[nodiscard]] static bool exist(const Application& app, const std::string& name);
};
}