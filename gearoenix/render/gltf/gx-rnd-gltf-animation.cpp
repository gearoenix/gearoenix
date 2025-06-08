#include "../../core/ecs/gx-cr-ecs-entity.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../physics/animation/gx-phs-anm-bone.hpp"
#include "../../physics/animation/gx-phs-anm-interpolation.hpp"
#include "../../physics/gx-phs-transformation.hpp"
#include "gx-rnd-gltf-context.hpp"
#include "gx-rnd-gltf-transform.hpp"

namespace {
[[nodiscard]] gearoenix::physics::animation::Interpolation convert_interpolation(const std::string& interpolation)
{
    if ("CUBICSPLINE" == interpolation) {
        return gearoenix::physics::animation::Interpolation::Gltf2CubicSPLine;
    }
    if ("LINEAR" == interpolation) {
        return gearoenix::physics::animation::Interpolation::Linear;
    }
    if ("STEP" == interpolation) {
        return gearoenix::physics::animation::Interpolation::Step;
    }
    GX_LOG_F("Unexpected interpolation " << interpolation);
}

template <template <typename> typename Value>
void read_output(
    boost::container::flat_map<double, gearoenix::physics::animation::Keyframe<Value<double>>>& keyframes,
    const tinygltf::Accessor& input,
    const tinygltf::Accessor& output,
    const tinygltf::BufferView& output_bv,
    const gearoenix::physics::animation::Interpolation interpolation,
    const std::uint64_t output_b_ptr,
    const std::vector<float>& times)
{
    keyframes.reserve(input.count);
    const auto output_bytes_count = output.count * sizeof(Value<float>);
    GX_COMPLAIN_D(output_bytes_count == output_bv.byteLength, "Inefficient gltf exporter for keyframes.");
    GX_ASSERT_D(output_bytes_count <= output_bv.byteLength);
    switch (interpolation) {
    case gearoenix::physics::animation::Interpolation::Gltf2CubicSPLine: {
        GX_ASSERT_D(input.count * sizeof(Value<float>) * 3 == output_bytes_count);
        for (std::uint64_t data_i = 0, curr_output_ptr = output_b_ptr;
            data_i < input.count; ++data_i, curr_output_ptr += sizeof(Value<float>)) {
            const std::uint64_t in_ptr = curr_output_ptr;
            const auto in = Value<double>(*reinterpret_cast<const Value<float>*>(in_ptr));
            curr_output_ptr += sizeof(Value<float>);
            const std::uint64_t key_ptr = curr_output_ptr;
            const auto key = Value<double>(*reinterpret_cast<const Value<float>*>(key_ptr));
            curr_output_ptr += sizeof(Value<float>);
            const std::uint64_t out_ptr = curr_output_ptr;
            const auto out = Value<double>(*reinterpret_cast<const Value<float>*>(out_ptr));
            keyframes.emplace(
                times[data_i],
                gearoenix::physics::animation::Keyframe<Value<double>>::construct_gltf2_bezier(in, key, out));
        }
        break;
    }
    case gearoenix::physics::animation::Interpolation::Linear: {
        GX_ASSERT_D(input.count * sizeof(Value<float>) == output_bytes_count);
        for (std::uint64_t data_i = 0, curr_output_ptr = output_b_ptr;
            data_i < input.count; ++data_i, curr_output_ptr += sizeof(Value<float>)) {
            keyframes.emplace(
                times[data_i],
                gearoenix::physics::animation::Keyframe<Value<double>>::construct_linear(
                    Value<double>(*reinterpret_cast<const Value<float>*>(curr_output_ptr))));
        }
        break;
    }
    case gearoenix::physics::animation::Interpolation::Step: {
        GX_ASSERT_D(input.count * sizeof(Value<float>) == output_bytes_count);
        for (std::uint64_t data_i = 0, curr_output_ptr = output_b_ptr;
            data_i < input.count; ++data_i, curr_output_ptr += sizeof(Value<float>)) {
            keyframes.emplace(
                times[data_i],
                gearoenix::physics::animation::Keyframe<Value<double>>::construct_step(
                    Value<double>(*reinterpret_cast<const Value<float>*>(curr_output_ptr))));
        }
        break;
    }
    default: {
        GX_UNEXPECTED;
    }
    }
}
}

gearoenix::render::gltf::Animations::Animations(const Context& context)
    : context(context)
{
}

gearoenix::render::gltf::Animations::~Animations() = default;

void gearoenix::render::gltf::Animations::load()
{
    for (const tinygltf::Animation& anm : context.data.animations) {
        for (const tinygltf::AnimationChannel& chn : anm.channels) {
            GX_ASSERT_D(is_bone(chn.target_node));
            auto* const bone_entity = get(chn.target_node);
            const auto& smp = anm.samplers[chn.sampler];
            const auto interpolation = convert_interpolation(smp.interpolation);
            const auto& input = context.data.accessors[smp.input];
            const auto& output = context.data.accessors[smp.output];
            GX_ASSERT_D(input.type == TINYGLTF_TYPE_SCALAR);
            GX_ASSERT_D(input.count > 0);
            GX_ASSERT_D(output.count > 0);
            GX_ASSERT_D((interpolation != physics::animation::Interpolation::Gltf2CubicSPLine && input.count == output.count) || (interpolation == physics::animation::Interpolation::Gltf2CubicSPLine && (input.count * 3) == output.count));
            std::vector<float> times(input.count);
            const auto& input_bv = context.data.bufferViews[input.bufferView];
            const auto& output_bv = context.data.bufferViews[output.bufferView];
            const auto input_byte_length = input.count * sizeof(float);
            GX_COMPLAIN_D(input_byte_length == input_bv.byteLength, "Wast of memory in gltf animation sample, probably inefficient exporter");
            GX_ASSERT_D(input_byte_length <= input_bv.byteLength);
            GX_ASSERT_D(0 == input_bv.byteStride);
            GX_ASSERT_D(0 == output_bv.byteStride);
            std::memcpy(times.data(), &context.data.buffers[input_bv.buffer].data[input_bv.byteOffset + input.byteOffset], input_byte_length);
            const auto& output_b = context.data.buffers[output_bv.buffer].data;
            const auto output_b_ptr = reinterpret_cast<std::uint64_t>(&output_b[output_bv.byteOffset + output.byteOffset]);
            if ("translation" == chn.target_path) {
                GX_ASSERT_D(output.type == TINYGLTF_TYPE_VEC3);
                read_output(bone_entity->get_component<physics::animation::Bone>()->get_translation_samples(), input, output, output_bv, interpolation, output_b_ptr, times);
            } else if ("rotation" == chn.target_path) {
                GX_ASSERT_D(output.type == TINYGLTF_TYPE_VEC4);
                read_output(bone_entity->get_component<physics::animation::Bone>()->get_rotation_samples(), input, output, output_bv, interpolation, output_b_ptr, times);
            } else if ("scale" == chn.target_path) {
                GX_ASSERT_D(output.type == TINYGLTF_TYPE_VEC3);
                read_output(bone_entity->get_component<physics::animation::Bone>()->get_scale_samples(), input, output, output_bv, interpolation, output_b_ptr, times);
            } else {
                GX_UNEXPECTED;
            }
        }
    }
}

int gearoenix::render::gltf::Animations::find_parent(const int child) const
{
    for (int i = 0; i < context.data.nodes.size(); ++i) {
        for (const auto& n = context.data.nodes[i]; const auto c : n.children) {
            if (child == c) {
                return i;
            }
        }
    }
    return -1;
}

gearoenix::core::ecs::Entity* gearoenix::render::gltf::Animations::get(const int node_index)
{
    if (node_index < 0 || !is_bone(node_index)) {
        return nullptr;
    }

    std::lock_guard bone_entities_lock_guard(bone_entities_lock);
    if (const auto search = bone_entities.find(node_index); bone_entities.end() != search) {
        return search->second.get();
    }

    auto* const parent = get(find_parent(node_index));
    auto bone_entity = physics::animation::Bone::build(std::string(context.data.nodes[node_index].name), parent);
    apply_transform(node_index, context, *bone_entity->get_component<physics::Transformation>());
    return bone_entities.emplace(node_index, std::move(bone_entity)).first->second.get();
}

bool gearoenix::render::gltf::Animations::is_bone(const int node_index) const
{
    const auto& n = context.data.nodes[node_index];
    return n.skin == -1 && n.mesh == -1 && n.camera == -1 && n.extensions.empty() && n.light == -1 && n.emitter == -1 && !context.armatures.is_armature(node_index);
}
