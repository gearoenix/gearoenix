#ifndef GEAROENIX_DIRECT3DX_ENGINE_HPP
#define GEAROENIX_DIRECT3DX_ENGINE_HPP
#include "../render/gx-rnd-build-configuration.hpp"
#ifdef GX_RENDER_DIRECT3DX_ENABLED
#include "../render/engine/gx-rnd-eng-engine.hpp"
#include "gx-d3d-loader.hpp"

namespace gearoenix::platform {
struct Application;
}

namespace gearoenix::direct3dx {
struct Adapter;
struct Device;
struct Queue;
struct Swapchain;

struct Viewport final {
    float left = -1.0f;
    float top = -1.0f;
    float right = 1.0f;
    float bottom = 1.0f;
};

struct RayGenConstantBuffer final {
    Viewport viewport;
    Viewport stencil;
};

struct Engine final : public render::engine::Engine {
    RayGenConstantBuffer ray_gen_cb;
    GX_GET_RRF_PRV(platform::Application, platform_application)
    GX_GET_CREF_PRV(std::shared_ptr<Adapter>, adapter)
    GX_GET_CREF_PRV(std::shared_ptr<Device>, device)
    GX_GET_CREF_PRV(std::shared_ptr<Queue>, queue)
    GX_GET_CREF_PRV(std::shared_ptr<Swapchain>, swapchain)
    Microsoft::WRL::ComPtr<ID3D12RootSignature> raytracing_global_root_signature;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> raytracing_local_root_signature;
    Microsoft::WRL::ComPtr<ID3D12StateObject> state_object;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptor_heap;
    Microsoft::WRL::ComPtr<ID3D12Resource> index_buffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertex_buffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> bottom_level_acceleration_structure;
    Microsoft::WRL::ComPtr<ID3D12Resource> top_level_acceleration_structure;
    Microsoft::WRL::ComPtr<ID3D12Resource> ray_gen_shader_table;
    Microsoft::WRL::ComPtr<ID3D12Resource> miss_shader_table;
    Microsoft::WRL::ComPtr<ID3D12Resource> hit_group_shader_table;
    Microsoft::WRL::ComPtr<ID3D12Resource> raytracing_output;
    UINT raytracing_output_resource_uav_descriptor_heap_index = static_cast<UINT>(-1);
    D3D12_GPU_DESCRIPTOR_HANDLE raytracing_output_resource_uav_gpu_descriptor { 0 };
    UINT descriptor_size = static_cast<UINT>(-1);
    UINT descriptors_allocated = 0;

private:
    explicit Engine(platform::Application& platform_application) noexcept;
    void device_lost_handle(int failed_tries = 0) noexcept;
    void window_resized(int failed_tries = 0) noexcept;
    void initialize_ray_tracing_resources() noexcept;
    void create_root_signatures() noexcept;
    void serialize_and_create_raytracing_root_signature(
        const D3D12_ROOT_SIGNATURE_DESC& desc,
        Microsoft::WRL::ComPtr<ID3D12RootSignature>& root_sig) noexcept;
    void create_raytracing_pipeline_state_object() noexcept;
    void create_local_root_signature_subobjects(CD3DX12_STATE_OBJECT_DESC&) const noexcept;
    void create_descriptor_heap() noexcept;
    void build_geometry() noexcept;
    void allocate_upload_buffer(
        const void* data,
        UINT64 data_size,
        Microsoft::WRL::ComPtr<ID3D12Resource>& resource,
        const wchar_t* resource_name) noexcept;
    void allocate_uav_buffer(
        UINT64 buffer_size,
        Microsoft::WRL::ComPtr<ID3D12Resource>& resource,
        D3D12_RESOURCE_STATES initial_resource_state,
        const wchar_t* resource_name) noexcept;
    void build_acceleration_structures() noexcept;
    void build_shader_tables() noexcept;
    void create_raytracing_output_resource() noexcept;
    [[nodiscard]] UINT allocate_descriptor(D3D12_CPU_DESCRIPTOR_HANDLE& cpu_descriptor, UINT descriptor_index_to_use) noexcept;

public:
    Engine(Engine&&) = delete;
    Engine(const Engine&) = delete;
    Engine& operator=(Engine&&) = delete;
    Engine& operator=(const Engine&) = delete;
    ~Engine() noexcept final;
    [[nodiscard]] static bool is_supported() noexcept;
    [[nodiscard]] static std::unique_ptr<Engine> construct(platform::Application& platform_application) noexcept;

    void start_frame() noexcept final;
    void end_frame() noexcept final;
    void upload_imgui_fonts() noexcept final;
};
}

#endif
#endif