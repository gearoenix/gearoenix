#pragma once
#include "gx-plt-stm-stream.hpp"
#include <vector>

namespace gearoenix::platform::stream {
struct Memory final : public Stream {
private:
    std::vector<unsigned char> mem_data;
    std::uint64_t index = 0;

public:
    Memory();
    ~Memory() final;
    [[nodiscard]] std::uint64_t read(void* d, std::uint64_t length) final;
    [[nodiscard]] std::uint64_t write(const void* d, std::uint64_t length) final;
    void seek(std::uint64_t offset) final;
    [[nodiscard]] std::uint64_t tell() final;
    [[nodiscard]] std::uint64_t size() final;
    void flush() final;
};
}