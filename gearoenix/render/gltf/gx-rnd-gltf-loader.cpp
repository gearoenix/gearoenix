#include "gx-rnd-gltf-loader.hpp"
#define TINYGLTF_IMPLEMENTATION
#include "../../platform/stream/gx-plt-stm-path.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../scene/gx-rnd-scn-builder.hpp"
#include "../scene/gx-rnd-scn-manager.hpp"
#include "gx-rnd-gltf-context.hpp"

namespace gearoenix::render::gltf {
void load_scenes(
    const std::shared_ptr<Context>& ctx,
    const core::job::EndCaller<std::vector<std::shared_ptr<scene::Builder>>>& scenes_end_callback,
    const core::job::EndCaller<>& entity_end_callback)
{
    scenes_end_callback.set_return(std::vector<std::shared_ptr<scene::Builder>>(ctx->data.scenes.size()));

    ctx->animations.load(entity_end_callback);

    core::job::EndCaller meshes_ready([ctx, scenes_end_callback, entity_end_callback] {
        const core::job::EndCaller gpu_end_callback([ctx, s = scenes_end_callback] { (void)s; (void)ctx; });
        for (int index = 0; index < ctx->data.scenes.size(); ++index) {
            const tinygltf::Scene& scn = ctx->data.scenes[index];
            GX_LOG_D("Loading scene: " << scn.name);
            auto scene_builder = engine::Engine::get()->get_scene_manager()->build(scn.name, 0.0, core::job::EndCaller(entity_end_callback));
            for (const int scene_node_index : scn.nodes) {
                ctx->nodes.process(scene_node_index, nullptr, gpu_end_callback, entity_end_callback, scene_builder);
            }
            scenes_end_callback.get_return()[index] = std::move(scene_builder);
        }
    });

    core::job::EndCaller materials_ready([ctx, end = std::move(meshes_ready)]() mutable {
        ctx->meshes.load(std::move(end));
    });

    core::job::EndCaller textures_ready([ctx, end = std::move(materials_ready)]() mutable {
        ctx->materials.load(std::move(end));
    });

    ctx->textures.load(std::move(textures_ready));
}

void read_gltf(const std::shared_ptr<Context>& ctx, const platform::stream::Path& file)
{
    std::string err, warn;
    const auto stream = platform::stream::Stream::open(file, engine::Engine::get()->get_platform_application());
    GX_ASSERT(nullptr != stream);
    const auto file_data = stream->get_file_content();
    if (!ctx->context.LoadBinaryFromMemory(&ctx->data, &err, &warn, file_data.data(), static_cast<unsigned int>(file_data.size())) || !err.empty()) {
        GX_LOG_F("Error in GLTF loader: " << err);
    }
    if (!warn.empty()) {
        GX_LOG_E("Warning in GLTF loader: " << warn);
    }
}

void load(
    const platform::stream::Path& file,
    const core::job::EndCaller<std::vector<std::shared_ptr<scene::Builder>>>& scenes_end_callback,
    const core::job::EndCaller<>& entity_end_callback)
{
    core::job::send_job_to_pool([file = file, scenes_end_callback, entity_end_callback] {
        const auto context = std::make_shared<Context>();
        read_gltf(context, file);
        load_scenes(context, scenes_end_callback, entity_end_callback);
    });
}
}
