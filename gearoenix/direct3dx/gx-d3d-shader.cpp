#include "gx-d3d-shader.hpp"
#ifdef GX_RENDER_DIRECT3DX_ENABLED
#include "../core/gx-cr-string.hpp"
#include "../core/macro/gx-cr-mcr-assert.hpp"
#include "../math/gx-math-numeric.hpp"

gearoenix::direct3dx::ShaderRecord::ShaderRecord(
    void* const shader_identifier,
    const UINT shader_identifier_size) noexcept
    : shader_identifier { shader_identifier, shader_identifier_size }
{
}

gearoenix::direct3dx::ShaderRecord::ShaderRecord(
    void* const shader_identifier,
    const UINT shader_identifier_size,
    void* const local_root_arguments,
    const UINT local_root_arguments_size) noexcept
    : shader_identifier { shader_identifier, shader_identifier_size }
    , local_root_arguments { local_root_arguments, local_root_arguments_size }
{
}

void gearoenix::direct3dx::ShaderRecord::copy_to(void* const dest) const noexcept
{
    auto* const byte_dest = reinterpret_cast<std::uint8_t*>(dest);
    std::memcpy(byte_dest, shader_identifier.ptr, shader_identifier.size);
    if (nullptr != local_root_arguments.ptr) {
        std::memcpy(byte_dest + shader_identifier.size, local_root_arguments.ptr, local_root_arguments.size);
    }
}

gearoenix::direct3dx::ShaderTable::ShaderTable(
    ID3D12Device* const device,
    const UINT num_shader_records,
    const UINT shader_record_size,
    LPCWSTR resource_name) noexcept
    : shader_record_size(math::Numeric::align(shader_record_size, static_cast<UINT>(D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT)))
    , name(resource_name)
{
    shader_records.reserve(num_shader_records);
    const UINT buffer_size = num_shader_records * shader_record_size;
    allocate(device, buffer_size, resource_name);
    mapped_shader_records = map_cpu_write_only();
}

void gearoenix::direct3dx::ShaderTable::push_back(const ShaderRecord& shader_record) noexcept
{
    GX_ASSERT(shader_records.size() < shader_records.capacity())
    shader_records.push_back(shader_record);
    shader_record.copy_to(mapped_shader_records);
    mapped_shader_records += shader_record_size;
}

void gearoenix::direct3dx::ShaderTable::debug_print(std::map<const void*, std::wstring> shader_id_to_string_map) noexcept
{
    std::wstringstream w_str;
    w_str << L"|--------------------------------------------------------------------\n"
          << L"|Shader table - " << name << L": "
          << shader_record_size << L" | "
          << (shader_records.size() * shader_record_size) << L" bytes\n";
    for (UINT i = 0; i < shader_records.size(); ++i) {
        w_str << L"| [" << i << L"]: "
              << shader_id_to_string_map[shader_records[i].get_shader_identifier().ptr] << L", "
              << shader_records[i].get_shader_identifier().size << L" + "
              << shader_records[i].get_local_root_arguments().size << L" bytes \n";
    }
    w_str << L"|--------------------------------------------------------------------\n\n";
    GX_LOG_D(core::String::to_string(w_str.str()))
}

#endif