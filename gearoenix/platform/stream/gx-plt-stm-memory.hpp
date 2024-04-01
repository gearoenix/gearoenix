#ifndef GEAROENIX_SYSTEM_STREAM_MEMORY_HPP
#define GEAROENIX_SYSTEM_STREAM_MEMORY_HPP
#include "gx-plt-stm-stream.hpp"
#include <vector>

namespace gearoenix::platform::stream {
struct Memory final : public Stream {
private:
    std::vector<unsigned char> mem_data;
    std::size_t index = 0;

public:
    Memory();
    ~Memory() final;
    [[nodiscard]] std::size_t read(void* d, std::size_t length) final;
    [[nodiscard]] std::size_t write(const void* d, std::size_t length) final;
    void seek(std::size_t offset) final;
    [[nodiscard]] std::size_t tell() final;
    [[nodiscard]] std::size_t size() final;
    void flush() final;
};
}
#endif
