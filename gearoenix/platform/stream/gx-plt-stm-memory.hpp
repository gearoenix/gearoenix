#ifndef GEAROENIX_SYSTEM_STREAM_MEMORY_HPP
#define GEAROENIX_SYSTEM_STREAM_MEMORY_HPP

#include "gx-plt-stm-stream.hpp"
#include <vector>

namespace gearoenix::platform::stream {
struct Memory final : public Stream {
private:
    std::vector<unsigned char> mem_data;
    core::Count index = 0;

public:
    Memory() noexcept;
    ~Memory() noexcept final;
    [[nodiscard]] core::Count read(void* d, core::Count length) noexcept final;
    [[nodiscard]] core::Count write(const void* d, core::Count length) noexcept final;
    void seek(core::Count offset) noexcept final;
    [[nodiscard]] core::Count tell() noexcept final;
    [[nodiscard]] core::Count size() noexcept final;
};
}
#endif
