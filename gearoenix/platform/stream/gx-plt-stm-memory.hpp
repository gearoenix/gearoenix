#pragma once
#include "gx-plt-stm-stream.hpp"
#include <vector>

namespace gearoenix::platform::stream {
struct Memory final : Stream {
    GX_GET_CREF_PRV(std::vector<std::uint8_t>, mem_data);
    GX_GET_VAL_PRV(stream_size_t, index, 0);

    [[nodiscard]] stream_size_t read(void* d, stream_size_t length) override;
    [[nodiscard]] stream_size_t write(const void* d, stream_size_t length) override;
    void seek(stream_size_t offset) override;
    [[nodiscard]] stream_size_t tell() override;
    [[nodiscard]] stream_size_t size() override;
    void flush() override;
    [[nodiscard]] std::vector<std::uint8_t> get_file_content() override;

public:
    Memory();
    explicit Memory(std::vector<std::uint8_t>&& data);
    ~Memory() override;
};
}