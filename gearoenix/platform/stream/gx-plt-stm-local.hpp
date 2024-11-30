#pragma once
#include "gx-plt-stm-stream.hpp"
#include <fstream>

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::platform::stream {
struct Local : public Stream {
private:
    std::fstream file;
    explicit Local(std::fstream file);

public:
    Local(const Application& app, const std::string& name, bool writable = false);
    ~Local() override;
    [[nodiscard]] static Local* open(const Application& app, const std::string& name, bool writable = false);
    [[nodiscard]] std::uint64_t read(void* data, std::uint64_t length) override;
    [[nodiscard]] std::uint64_t write(const void* data, std::uint64_t length) override;
    void seek(std::uint64_t offset) override;
    [[nodiscard]] std::uint64_t tell() override;
    [[nodiscard]] std::uint64_t size() override;
    void flush() override;
    [[nodiscard]] static bool exist(const Application& app, const std::string& name);
};
}