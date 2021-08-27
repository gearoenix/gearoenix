#ifndef GEAROENIX_DIRECT3DX_SHADER_HPP
#define GEAROENIX_DIRECT3DX_SHADER_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3DX_ENABLED
#include "gx-d3d-buffer.hpp"
#include <map>
#include <string>

namespace gearoenix::direct3dx {
// Shader record = {{Shader ID}, {RootArguments}}
struct ShaderRecord final {
    struct PointerWithSize {
        const void* const ptr = nullptr;
        const UINT size = 0;
    };

    GX_GET_CREF_PRV(PointerWithSize, shader_identifier)
    GX_GET_CREF_PRV(PointerWithSize, local_root_arguments)
public:
    ShaderRecord(void* shader_identifier, UINT shader_identifier_size) noexcept;
    ShaderRecord(
        void* shader_identifier,
        UINT shader_identifier_size,
        void* local_root_arguments,
        UINT local_root_arguments_size) noexcept;
    void copy_to(void* dest) const noexcept;
};

struct ShaderTable final : public GpuUploadBuffer {
    GX_GET_VAL_PRV(UINT, shader_record_size, 0)
private:
    std::uint8_t* mapped_shader_records;
    std::wstring name;
    std::vector<ShaderRecord> shader_records;

public:
    ShaderTable() = delete;
    ShaderTable(
        ID3D12Device* device,
        UINT num_shader_records,
        UINT shader_record_size,
        LPCWSTR resource_name) noexcept;
    void push_back(const ShaderRecord& shader_record) noexcept;
    // Pretty-print the shader records.
    void debug_print(std::map<const void*, std::wstring> shader_id_to_string_map) noexcept;
};
}

#endif
#endif