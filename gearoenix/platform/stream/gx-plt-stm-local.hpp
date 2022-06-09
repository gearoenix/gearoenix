#ifndef GEAROENIX_SYSTEM_STREAM_LOCAL_HPP
#define GEAROENIX_SYSTEM_STREAM_LOCAL_HPP

#include "gx-plt-stm-stream.hpp"
#include <fstream>

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::platform::stream {
struct Local final : public Stream {
private:
    std::fstream file;
    explicit Local(std::fstream file) noexcept;

public:
    Local(const Application& app, const std::string& name, bool writable = false) noexcept;
    ~Local() noexcept final;
    [[nodiscard]] static Local* open(const Application& app, const std::string& name, bool writable = false) noexcept;
    [[nodiscard]] std::size_t read(void* data, std::size_t length) noexcept final;
    [[nodiscard]] std::size_t write(const void* data, std::size_t length) noexcept final;
    void seek(std::size_t offset) noexcept final;
    [[nodiscard]] std::size_t tell() noexcept final;
    [[nodiscard]] std::size_t size() noexcept final;
    [[nodiscard]] static bool exist(const Application& app, const std::string& name) noexcept;
};
}
#endif
