#include "../../core/macro/gx-cr-mcr-assert.hpp"
#include "../../physics/animation/gx-phs-anm-channel.hpp"
#include "../../physics/animation/gx-phs-anm-interpolation.hpp"
#include "gx-rnd-gltf-context.hpp"

namespace {
/// It can not be used everywhere, it has its own context. It must be used in bone tracing code.
[[nodiscard]] bool is_gltf_node_a_bone(const tinygltf::Node& n)
{
    return n.skin == -1 && n.mesh == -1 && n.camera == -1 && n.extensions.empty();
}

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
    std::vector<std::pair<double, gearoenix::physics::animation::Keyframe<Value<double>>>>& keyframes,
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
            keyframes.emplace_back(
                times[data_i],
                gearoenix::physics::animation::Keyframe<Value<double>> {
                    gearoenix::physics::animation::KeyframeGltf2Bezier<Value<double>> {
                        .key = key,
                        .in = in,
                        .out = out,
                    } });
        }
        break;
    }
    case gearoenix::physics::animation::Interpolation::Linear: {
        GX_ASSERT_D(input.count * sizeof(Value<float>) == output_bytes_count);
        for (std::uint64_t data_i = 0, curr_output_ptr = output_b_ptr;
            data_i < input.count; ++data_i, curr_output_ptr += sizeof(Value<float>)) {
            keyframes.emplace_back(
                times[data_i],
                gearoenix::physics::animation::KeyframeLinear<Value<double>> {
                    Value<double>(*reinterpret_cast<const Value<float>*>(curr_output_ptr)) });
        }
        break;
    }
    case gearoenix::physics::animation::Interpolation::Step: {
        GX_ASSERT_D(input.count * sizeof(Value<float>) == output_bytes_count);
        for (std::uint64_t data_i = 0, curr_output_ptr = output_b_ptr;
            data_i < input.count; ++data_i, curr_output_ptr += sizeof(Value<float>)) {
            keyframes.emplace_back(
                times[data_i],
                gearoenix::physics::animation::KeyframeStep<Value<double>> {
                    Value<double>(*reinterpret_cast<const Value<float>*>(curr_output_ptr)) });
        }
        break;
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
            const auto& bone_node = context.data.nodes[chn.target_node];
            GX_ASSERT_D(is_gltf_node_a_bone(bone_node));
            auto& bone_channels = *bones_channels.emplace(chn.target_node, std::make_unique<physics::animation::BoneChannelBuilder>()).first->second;
            bone_channels.target_bone = bone_node.name;
            const tinygltf::AnimationSampler& smp = anm.samplers[chn.sampler];
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
            std::memcpy(times.data(), &context.data.buffers[input_bv.buffer].data[input_bv.byteOffset], input_byte_length);
            const auto& output_b = context.data.buffers[output_bv.buffer].data;
            const auto output_b_ptr = reinterpret_cast<std::uint64_t>(&output_b[output_bv.byteOffset]);
            if ("translation" == chn.target_path) {
                GX_ASSERT_D(output.type == TINYGLTF_TYPE_VEC3);
                read_output(bone_channels.translation_samples, input, output, output_bv, interpolation, output_b_ptr, times);
            } else if ("rotation" == chn.target_path) {
                GX_ASSERT_D(output.type == TINYGLTF_TYPE_VEC4);
                read_output(bone_channels.rotation_samples, input, output, output_bv, interpolation, output_b_ptr, times);
            } else if ("scale" == chn.target_path) {
                GX_ASSERT_D(output.type == TINYGLTF_TYPE_VEC3);
                read_output(bone_channels.scale_samples, input, output, output_bv, interpolation, output_b_ptr, times);
            } else {
                GX_UNEXPECTED;
            }
        }
    }
}
