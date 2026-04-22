#include "gx-rnd-buf-manager.hpp"
#include "../../core/allocator/gx-cr-alc-range.hpp"
#include "../../math/gx-math-numeric.hpp"
#include "../camera/gx-rnd-cmr-camera.hpp"
#include "../camera/gx-rnd-cmr-uniform.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../light/gx-rnd-lt-directional.hpp"
#include "../light/gx-rnd-lt-point.hpp"
#include "../light/gx-rnd-lt-uniform.hpp"
#include "../material/gx-rnd-mat-pbr.hpp"
#include "../material/gx-rnd-mat-sprite.hpp"
#include "../material/gx-rnd-mat-uniform.hpp"
#include "../material/gx-rnd-mat-unlit.hpp"
#include "../model/gx-rnd-mdl-model.hpp"
#include "../model/gx-rnd-mdl-uniform.hpp"
#include "../record/gx-rnd-rcd-camera.hpp"
#include "../reflection/gx-rnd-rfl-probe.hpp"
#include "../reflection/gx-rnd-rfl-uniform.hpp"
#include "../scene/gx-rnd-scn-scene.hpp"
#include "../scene/gx-rnd-scn-uniform.hpp"
#include "gx-rnd-buf-uniform.hpp"

namespace {
constexpr auto uniform_max_counts = [] {
    using Idx = gearoenix::render::buffer::UniformRegionIndex;
    std::array<std::int64_t, static_cast<std::size_t>(Idx::count)> a { };
    a[static_cast<std::size_t>(Idx::bones)] = static_cast<std::int64_t>(50 * GX_RENDER_MAX_BONES_COUNT);
    a[static_cast<std::size_t>(Idx::cameras)] = static_cast<std::int64_t>(gearoenix::render::camera::Camera::max_count);
    a[static_cast<std::size_t>(Idx::camera_joint_model)] = static_cast<std::int64_t>(gearoenix::render::record::Camera::cameras_joint_models_max_count);
    a[static_cast<std::size_t>(Idx::directional_lights)] = static_cast<std::int64_t>(gearoenix::render::light::Directional::max_count) + static_cast<std::int64_t>(gearoenix::render::light::ShadowCasterDirectional::max_count);
    a[static_cast<std::size_t>(Idx::point_lights)] = static_cast<std::int64_t>(gearoenix::render::light::Point::max_count);
    a[static_cast<std::size_t>(Idx::shadow_caster_directional_lights)] = static_cast<std::int64_t>(gearoenix::render::light::ShadowCasterDirectional::max_count);
    a[static_cast<std::size_t>(Idx::materials)] = static_cast<std::int64_t>(gearoenix::render::material::Pbr::max_count) + static_cast<std::int64_t>(gearoenix::render::material::Unlit::max_count) + static_cast<std::int64_t>(gearoenix::render::material::Sprite::max_count);
    a[static_cast<std::size_t>(Idx::models)] = static_cast<std::int64_t>(gearoenix::render::model::Model::max_count);
    a[static_cast<std::size_t>(Idx::reflection_probes)] = static_cast<std::int64_t>(gearoenix::render::reflection::Probe::max_count);
    a[static_cast<std::size_t>(Idx::scenes)] = static_cast<std::int64_t>(gearoenix::render::scene::Scene::max_count);
    return a;
}();

constexpr std::array uniform_item_raw_sizes = {
    sizeof(GxShaderDataBone),
    sizeof(GxShaderDataCamera),
    sizeof(GxShaderDataCameraJointModel),
    sizeof(GxShaderDataDirectionalLight),
    sizeof(GxShaderDataPointLight),
    sizeof(GxShaderDataShadowCasterDirectionalLight),
    sizeof(GxShaderDataMaterial),
    sizeof(GxShaderDataModel),
    sizeof(GxShaderDataReflectionProbe),
    sizeof(GxShaderDataScene),
};

std::array<std::int64_t, static_cast<std::size_t>(gearoenix::render::buffer::UniformRegionIndex::count)> uniform_item_sizes { };
std::array<void*, static_cast<std::size_t>(gearoenix::render::buffer::UniformRegionIndex::count)> uniform_regions_pointers { };
std::array<std::shared_ptr<gearoenix::core::allocator::Range>, static_cast<std::size_t>(gearoenix::render::buffer::UniformRegionIndex::count)> uniform_regions;

/// One pending-writes byte buffer per in-flight frame (Vulkan: frames_in_flight,
/// OpenGL: 1). Backends size this directly in their constructor. Each entry encodes
/// <offset:size_t><size: size_t><bytes...>. Drained per-frame via apply_pending_writes
/// so that a single CPU-side upload reaches every in-flight frame.
std::vector<std::vector<std::uint8_t>> frame_uniform_upload_datas;

/// Guards frame_datas. Upload_to_all_frames may be called from multiple worker threads,
/// while apply_pending_writes drains under the same lock to avoid losing writes that
/// race with a frame boundary.
std::mutex frame_uniform_upload_datas_lock;

void set_uniform_base_ptr(const std::size_t ptr)
{
    gearoenix::render::buffer::Uniform::set_frame_pointer(ptr);
    for (std::size_t i = 0; i < uniform_regions_pointers.size(); ++i) {
        uniform_regions_pointers[i] = reinterpret_cast<void*>(ptr + uniform_regions[i]->get_offset());
    }
}
}

std::int64_t gearoenix::render::buffer::Manager::uniform_range_alignment = 1;
std::int64_t gearoenix::render::buffer::Manager::uniform_internal_range_alignment = 1;
std::shared_ptr<gearoenix::core::allocator::Range> gearoenix::render::buffer::Manager::uniform_root_range;

gearoenix::render::buffer::Manager::Manager()
    : Singleton(this)
{
    const auto frames_count = static_cast<std::size_t>(engine::Engine::get_frames_count());
    GX_ASSERT_D(frames_count > 0 && frames_count < 5); // make sure it has the right values.
    frame_uniform_upload_datas.resize(frames_count);
}

void gearoenix::render::buffer::Manager::initialise_ranges()
{
    for (std::size_t i = 0; i < static_cast<std::size_t>(UniformRegionIndex::count); ++i) {
        uniform_item_sizes[i] = math::Numeric::align(static_cast<std::int64_t>(uniform_item_raw_sizes[i]), uniform_internal_range_alignment);
    }

    // Bone uniforms are handled separately because WebGL2 can bind only one uniform range at a time,
    // and each bound range may contain data for multiple bones.
    // On backends such as Vulkan, bone data lives in a large bindless allocation, so no extra per-item
    // alignment is required. If a future backend combines bindless access with strict internal range
    // alignment, the bone uniform allocation path must be revised accordingly.
    // And we have to consider the shader-data-indexing that a uniform provides.
    constexpr auto bone_reg_idx = static_cast<std::size_t>(UniformRegionIndex::bones);
    uniform_item_sizes[bone_reg_idx] = static_cast<std::int64_t>(uniform_item_raw_sizes[bone_reg_idx]);

    std::array<std::int64_t, static_cast<std::size_t>(UniformRegionIndex::count)> regions_sizes { };
    std::int64_t total_size = 0;
    for (std::size_t i = 0; i < static_cast<std::size_t>(UniformRegionIndex::count); ++i) {
        const auto sz = uniform_item_sizes[i] * uniform_max_counts[i];
        GX_ASSERT_D(sz > 0);
        GX_ASSERT_D(sz % uniform_internal_range_alignment == 0);
        regions_sizes[i] = sz;
        total_size += math::Numeric::align(sz, uniform_range_alignment);
    }

    uniform_root_range = core::allocator::Range::construct(total_size);

    std::size_t region_index = 0;
    for (const auto sz : regions_sizes) {
        uniform_regions[region_index] = uniform_root_range->allocate(sz, uniform_range_alignment);
        GX_ASSERT_D(uniform_regions[region_index]);
        ++region_index;
    }
}

gearoenix::render::buffer::Manager::uniform_cns_args_t gearoenix::render::buffer::Manager::create_uniform_cns_args(
    std::shared_ptr<core::allocator::Range>&& range, const UniformRegionIndex idx)
{
    const auto region_off = range->get_offset() - uniform_regions[static_cast<std::size_t>(idx)]->get_offset();
    const auto shader_index = region_off / uniform_item_sizes[static_cast<std::size_t>(idx)];
    GX_ASSERT_D(region_off % uniform_item_sizes[static_cast<std::size_t>(idx)] == 0);
    return { std::move(range), static_cast<std::uint32_t>(shader_index) };
}

gearoenix::render::buffer::Manager::uniform_cns_args_t gearoenix::render::buffer::Manager::get_range(const std::int64_t sz, const UniformRegionIndex i)
{
    GX_ASSERT_D(i < UniformRegionIndex::count);
    const auto& region = uniform_regions[static_cast<std::size_t>(i)];
    GX_ASSERT_D(region);
    GX_ASSERT_D(sz > 0);
    auto range = region->allocate(sz, uniform_internal_range_alignment);
    GX_ASSERT_D(range);
    return create_uniform_cns_args(std::move(range), i);
}

gearoenix::render::buffer::Manager::~Manager() = default;

gearoenix::render::buffer::Manager::uniform_cns_args_t gearoenix::render::buffer::Manager::get_bones_range(const std::int64_t bones_count)
{
    const auto sz = static_cast<std::int64_t>(sizeof(GxShaderDataBone)) * bones_count;
    return get_range(sz, UniformRegionIndex::bones);
}

gearoenix::render::buffer::Manager::uniform_cns_args_t gearoenix::render::buffer::Manager::get_range(const UniformRegionIndex i)
{
    GX_ASSERT_D(i != UniformRegionIndex::bones); // We have different root for bones
    const auto sz = uniform_item_sizes[static_cast<std::size_t>(i)];
    return get_range(sz, i);
}

const std::shared_ptr<gearoenix::core::allocator::Range>& gearoenix::render::buffer::Manager::get_region(const UniformRegionIndex i)
{
    return uniform_regions[static_cast<std::size_t>(i)];
}

void gearoenix::render::buffer::Manager::start_frame()
{
    const auto ptr = get_uniform_frame_pointer();

    set_uniform_base_ptr(ptr);

    const auto frame_number = engine::Engine::get_frame_number();
    const std::lock_guard lock(frame_uniform_upload_datas_lock);
    GX_ASSERT_D(frame_number < frame_uniform_upload_datas.size());
    auto& fd = frame_uniform_upload_datas[frame_number];
    const auto* cursor = fd.data();
    const auto* const end = cursor + fd.size();
    while (cursor < end) {
        std::size_t off = 0;
        std::size_t sz = 0;
        std::memcpy(&off, cursor, sizeof(std::size_t));
        cursor += sizeof(std::size_t);
        std::memcpy(&sz, cursor, sizeof(std::size_t));
        cursor += sizeof(std::size_t);
        std::memcpy(reinterpret_cast<void*>(ptr + off), cursor, sz);
        cursor += sz;
    }
    GX_ASSERT_D(cursor == end);
    fd.clear();
}

void gearoenix::render::buffer::Manager::end_frame()
{
    // set_uniform_base_ptr(0);
}

void* gearoenix::render::buffer::Manager::get_uniform_region_raw_ptr(const UniformRegionIndex i)
{
    return uniform_regions_pointers[static_cast<std::size_t>(i)];
}

void gearoenix::render::buffer::Manager::upload_to_all_frames_uniforms(const std::size_t offset_from_frame_start, const void* const data, const std::size_t size)
{
    GX_ASSERT_D(size > 0);
    GX_ASSERT_D(data != nullptr);
    constexpr auto header_size = 2 * sizeof(std::size_t);
    const std::lock_guard lock(frame_uniform_upload_datas_lock);
    GX_ASSERT_D(!frame_uniform_upload_datas.empty());
    for (auto& fd : frame_uniform_upload_datas) {
        const auto base = fd.size();
        fd.resize(base + header_size + size);
        auto* const dst = fd.data() + base;
        std::memcpy(dst, &offset_from_frame_start, sizeof(std::size_t));
        std::memcpy(dst + sizeof(std::size_t), &size, sizeof(std::size_t));
        std::memcpy(dst + header_size, data, size);
    }
}
