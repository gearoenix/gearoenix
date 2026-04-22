#pragma once
#include "../../core/gx-cr-singleton.hpp"

#include <cstddef>
#include <memory>
#include <mutex>
#include <vector>

namespace gearoenix::core::allocator {
struct Range;
}

namespace gearoenix::render::buffer {
enum struct UniformRegionIndex : std::uint8_t {
    bones = 0,
    cameras = 1,
    camera_joint_model = 2,
    directional_lights = 3,
    point_lights = 4,
    shadow_caster_directional_lights = 5,
    materials = 6,
    models = 7,
    reflection_probes = 8,
    scenes = 9,
    count = 10,
};

struct Uniform;
struct Manager : core::Singleton<Manager> {
    using uniform_cns_args_t = std::pair<std::shared_ptr<core::allocator::Range>, std::uint32_t>;

protected:
    static int64_t uniform_range_alignment;
    static int64_t uniform_internal_range_alignment;
    static std::shared_ptr<core::allocator::Range> uniform_root_range;

    Manager();

    /// This should be called after initialising the alignments
    static void initialise_ranges();

    [[nodiscard]] static uniform_cns_args_t create_uniform_cns_args(std::shared_ptr<core::allocator::Range>&& range, UniformRegionIndex);
    [[nodiscard]] static uniform_cns_args_t get_range(std::int64_t, UniformRegionIndex);
    [[nodiscard]] virtual std::size_t get_uniform_frame_pointer() = 0;

public:
    Manager(Manager&&) = delete;
    Manager(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    ~Manager() override;
    [[nodiscard]] static uniform_cns_args_t get_bones_range(std::int64_t bones_count);
    [[nodiscard]] static uniform_cns_args_t get_range(UniformRegionIndex);
    [[nodiscard]] static const std::shared_ptr<core::allocator::Range>& get_region(UniformRegionIndex);
    [[nodiscard]] static void* get_uniform_region_raw_ptr(UniformRegionIndex);
    virtual void start_frame();
    virtual void end_frame();

    /// Writes the same bytes at offset_from_frame_start into every in-flight frame's uniform buffer.
    /// Intended for data that rarely changes (materials, etc.) so that one write-on-change replaces
    /// a per-frame upload.
    static void upload_to_all_frames_uniforms(std::size_t offset_from_frame_start, const void* data, std::size_t size);

    template <typename T>
    static void upload_to_all_frames_uniforms(const std::size_t offset_from_frame_start, const T& value)
    {
        upload_to_all_frames_uniforms(offset_from_frame_start, &value, sizeof(T));
    }

    template <typename T>
    [[nodiscard]] static T* get_uniform_region_ptr(const UniformRegionIndex i)
    {
        return static_cast<T*>(get_uniform_region_raw_ptr(i));
    }
};
}