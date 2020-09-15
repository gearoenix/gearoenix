#include "gx-demo-wwr-count-down.hpp"
#include <gearoenix/core/asset/gx-cr-asset-manager.hpp>
#include <gearoenix/core/sync/gx-cr-sync-update-manager.hpp>
#include <gearoenix/physics/collider/gx-phs-cld-collider.hpp>
#include <gearoenix/render/engine/gx-rnd-eng-engine.hpp>
#include <gearoenix/render/model/gx-rnd-mdl-manager.hpp>
#include <gearoenix/render/scene/gx-rnd-scn-ui.hpp>
#include <gearoenix/render/widget/gx-rnd-wdg-text.hpp>
#include <gearoenix/system/gx-sys-app.hpp>

void gearoenix::demo::wwr::CountDown::update() noexcept
{
    if (!is_running)
        return;
    const auto t = e->get_delta_time();
    times_left -= t;
    if (times_left < 0.0) {
        is_running = false;
        unregister_update_function();
        texts[0]->get_transformation()->set_location(math::Vec3(0.0, 10.0, 0.0));
        for (const auto& text : texts)
            scene->remove_model(text->get_id());
        texts.clear();
        end();
        return;
    }
    const auto current_count_tmp = static_cast<int>(std::ceil(times_left) + 0.01);
    if (current_count_tmp != current_count) {
        texts[current_count - 1]->get_transformation()->set_location(math::Vec3(0.0, 10.0, 0.0));
        current_count = current_count_tmp;
    }
    const auto index = current_count_tmp - 1;
    const auto& text = texts[index];
    auto* const trn = text->get_transformation();
    const auto elapsed = static_cast<double>(current_count_tmp) - times_left;
    const auto y_pos = 0.7 - elapsed * 0.1;
    const auto scale = (0.2 + elapsed * 0.2) / text->get_collider()->get_current_local_scale().y;
    trn->set_location(math::Vec3(0.0, y_pos, 0.0));
    trn->local_scale(scale);
}

void gearoenix::demo::wwr::CountDown::loaded() noexcept
{
    std::vector<core::Id> texts_ids(texts.size());
    for (std::size_t i = 0; i < texts_ids.size(); ++i) {
        const auto& text = texts[i];
        scene->add_model(text);
        texts_ids[i] = text->get_id();
    }
    function_id = e->get_update_manager()->add(texts_ids, 0.0, [this] { update(); });
}

void gearoenix::demo::wwr::CountDown::unregister_update_function() noexcept
{
    if (function_id != 0) {
        e->get_update_manager()->remove(function_id);
        function_id = 0;
    }
}

gearoenix::demo::wwr::CountDown::CountDown() noexcept = default;

gearoenix::demo::wwr::CountDown::~CountDown() noexcept
{
    unregister_update_function();
}

std::shared_ptr<gearoenix::demo::wwr::CountDown> gearoenix::demo::wwr::CountDown::construct(
    std::shared_ptr<render::scene::Ui> s,
    std::function<void()> end,
    const std::size_t starting_count,
    const core::sync::EndCaller<core::sync::EndCallerIgnore>& render_ready_c) noexcept
{
    std::shared_ptr<CountDown> result(new CountDown());
    core::sync::EndCaller<core::sync::EndCallerIgnore> render_ready_call([result, render_ready_c] {
        result->loaded();
    });
    result->times_left = static_cast<double>(starting_count);
    result->current_count = static_cast<int>(starting_count);
    result->scene = std::move(s);
    result->texts.resize(starting_count);
    result->end = std::move(end);
    result->self = result;
    result->e = result->scene->get_e();
    auto* const e = result->scene->get_e();
    auto* const ast_mgr = e->get_system_application()->get_asset_manager();
    auto* const mdl_mgr = ast_mgr->get_model_manager();
    core::sync::EndCaller<render::widget::Text> txt_end([render_ready_call](const std::shared_ptr<render::widget::Text>&) {});
    for (std::size_t i = 0; i < starting_count;) {
        auto& text = result->texts[i];
        text = mdl_mgr->create<render::widget::Text>("count-down-" + std::to_string(i), txt_end);
        text->set_text_color(0.8, 0.2, 0.2, render_ready_call);
        text->set_text(std::to_wstring(++i), render_ready_call);
        auto* const trn = text->get_transformation();
        trn->set_location(math::Vec3(0.0, 10.0, 0.0));
        trn->local_scale(0.2);
    }
    return result;
}

void gearoenix::demo::wwr::CountDown::start() noexcept
{
    is_running = true;
}
