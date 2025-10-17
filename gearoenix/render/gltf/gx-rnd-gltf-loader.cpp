#include "gx-rnd-gltf-loader.hpp"
#include "../../platform/stream/gx-plt-stm-path.hpp"
#include "../../platform/stream/gx-plt-stm-stream.hpp"
#include "../engine/gx-rnd-eng-engine.hpp"
#include "../scene/gx-rnd-scn-manager.hpp"
#include "gx-rnd-gltf-context.hpp"

namespace gearoenix::render::gltf {
void load_scenes(
    const std::shared_ptr<Context>& ctx,
    const core::job::EndCaller<std::vector<core::ecs::EntityPtr>>& scenes_end_callback)
{
    scenes_end_callback.set_return(std::vector<core::ecs::EntityPtr>(ctx->data.scenes.size()));

    ctx->animations.load();

    core::job::EndCaller meshes_ready([ctx, scenes_end_callback] {
        const core::job::EndCaller gpu_end_callback([ctx, s = scenes_end_callback] { (void)s; (void)ctx; });
        for (int index = 0; index < ctx->data.scenes.size(); ++index) {
            const auto& scn = ctx->data.scenes[index];
            GX_LOG_D("Loading scene: " << scn.name);
            auto scene_entity = scene::Manager::get().build(std::string(scn.name), 0.0);
            for (const int scene_node_index : scn.nodes) {
                ctx->nodes.process(scene_node_index, scene_entity.get(), gpu_end_callback);
            }
            scenes_end_callback.get_return()[index] = std::move(scene_entity);
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

void read_gltf(const std::shared_ptr<Context>& ctx, platform::stream::Stream& stream)
{
    std::string err, warn;
    const auto file_data = stream.get_file_content();
    if (!ctx->context.LoadBinaryFromMemory(&ctx->data, &err, &warn, file_data.data(), static_cast<unsigned int>(file_data.size())) || !err.empty()) {
        GX_LOG_F("Error in GLTF loader: " << err);
    }
    if (!warn.empty()) {
        GX_LOG_E("Warning in GLTF loader: " << warn);
    }
}

void read_gltf(const std::shared_ptr<Context>& ctx, const platform::stream::Path& file)
{
    std::string err, warn;
    if (const auto& raw_path = file.get_raw_data(); raw_path.ends_with(".gltf")) {
        if (!ctx->context.LoadASCIIFromFile(&ctx->data, &err, &warn, raw_path)) {
            GX_LOG_F("Error in GLTF loader: " << err);
        }
    } else if (raw_path.ends_with(".glb")) {
        const auto stream = platform::stream::Stream::open(file);
        GX_ASSERT(nullptr != stream);
        read_gltf(ctx, *stream);
    } else {
        GX_LOG_F("Error in GLTF loader: Unknown file type: " << raw_path);
    }
    if (!warn.empty()) {
        GX_LOG_E("Warning in GLTF loader: " << warn);
    }
}

void load(const platform::stream::Path& file, core::job::EndCaller<std::vector<core::ecs::EntityPtr>>&& scene_entities_end_callback)
{
    core::job::send_job_to_pool([file = file, end = std::move(scene_entities_end_callback)] {
        const auto context = std::make_shared<Context>();
        read_gltf(context, file);
        load_scenes(context, end);
    });
}

void load(std::shared_ptr<platform::stream::Stream>&& stream, core::job::EndCaller<std::vector<core::ecs::EntityPtr>>&& scene_entities_end_callback)
{
    core::job::send_job_to_pool([stream = std::move(stream), end = std::move(scene_entities_end_callback)] {
        const auto context = std::make_shared<Context>();
        read_gltf(context, *stream);
        load_scenes(context, end);
    });
}
}