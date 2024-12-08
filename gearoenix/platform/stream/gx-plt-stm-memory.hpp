#pragma once
#include "gx-plt-stm-stream.hpp"
#include <vector>

namespace gearoenix::platform::stream {
struct Memory final : Stream {
    GX_GET_CREF_PRV(std::vector<std::uint8_t>, mem_data);
    GX_GET_VAL_PRV(std::uint64_t, index, 0);

    [[nodiscard]] std::uint64_t read(void* d, std::uint64_t length) override;
    [[nodiscard]] std::uint64_t write(const void* d, std::uint64_t length) override;
    void seek(std::uint64_t offset) override;
    [[nodiscard]] std::uint64_t tell() override;
    [[nodiscard]] std::uint64_t size() override;
    void flush() override;
    [[nodiscard]] std::vector<std::uint8_t> get_file_content() override;

public:
    Memory();
    ~Memory() override;
};
}