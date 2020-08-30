#include "gx-demo-wwr-count-down.hpp"
#include <gearoenix/render/scene/rnd-scn-ui.hpp>
#include <gearoenix/render/widget/rnd-wdg-text.hpp>
#include <gearoenix/render/engine/rnd-eng-engine.hpp>
#include <gearoenix/core/sync/gx-cr-sync-update-manager.hpp>
#include <gearoenix/system/sys-app.hpp>
#include <gearoenix/core/asset/cr-asset-manager.hpp>
#include <gearoenix/render/model/rnd-mdl-manager.hpp>

void gearoenix::demo::wwr::CountDown::update() noexcept {

}

gearoenix::demo::wwr::CountDown::CountDown() noexcept = default;

std::shared_ptr<gearoenix::demo::wwr::CountDown> gearoenix::demo::wwr::CountDown::construct(
        std::shared_ptr<render::scene::Ui> s,
        std::function<void()> end,
        const std::size_t starting_count,
        const core::sync::EndCaller <core::sync::EndCallerIgnore>& render_ready_call) noexcept {
    std::shared_ptr<CountDown> result(new CountDown());
    result->times_left = static_cast<double>(starting_count);
    result->current_count = starting_count;
    result->scene = std::move(s);
    result->texts.resize(starting_count);
    result->end = std::move(end);
    auto* const e = result->scene->get_e();
    auto* const ast_mgr = e->get_system_application()->get_asset_manager();
    auto* const mdl_mgr = ast_mgr->get_model_manager();
    core::sync::EndCaller<render::widget::Text> txt_end([render_ready_call](const std::shared_ptr<render::widget::Text>&){});
    std::vector<core::Id> texts_ids(starting_count);
    for(std::size_t i = 0 ; i < starting_count; ++i) {
        auto& text = result->texts[i];
        text = mdl_mgr->create<render::widget::Text>("count-down-" + std::to_string(i), txt_end);
        text->set_text_color(0.9, 0.1, 0.1, render_ready_call);
        text->set_text(std::to_wstring(i), render_ready_call);
        result->scene->add_model(text);
        texts_ids[i] = text->get_id();
    }
    result->function_id = e->get_update_manager()->add(texts_ids, 0.0, [result] { result->update(); });
    return result;
}
