#ifndef GX_DEMO_WWR_COUNT_DOWN_HPP
#define GX_DEMO_WWR_COUNT_DOWN_HPP

#include <functional>
#include <gearoenix/core/cr-static.hpp>
#include <gearoenix/core/sync/cr-sync-end-caller.hpp>
#include <memory>
#include <vector>

namespace gearoenix::render::engine {
class Engine;
}

namespace gearoenix::render::scene {
class Ui;
}

namespace gearoenix::render::widget {
class Text;
}

namespace gearoenix::demo::wwr {
class CountDown final {
    GX_GET_VAL_PRV(double, times_left, 3.0)
    GX_GET_VAL_PRV(int, current_count, static_cast<int>(times_left))
    GX_GET_CREF_PRV(std::shared_ptr<render::scene::Ui>, scene)
    GX_GET_CREF_PRV(std::vector<std::shared_ptr<render::widget::Text>>, texts)
    GX_GET_CREF_PRV(std::function<void()>, end)
    GX_GET_VAL_PRV(bool, is_running, false)
    GX_GET_VAL_PRV(core::Id, function_id, 0)
    GX_GET_CREF_PRV(std::weak_ptr<CountDown>, self)
    GX_GET_PTR_PRV(render::engine::Engine, e)
private:
    void update() noexcept;
    void loaded() noexcept;
    CountDown() noexcept;

public:
    [[nodiscard]] static std::shared_ptr<CountDown> construct(
        std::shared_ptr<render::scene::Ui> scene,
        std::function<void()> end,
        std::size_t starting_count = 3,
        const core::sync::EndCaller<core::sync::EndCallerIgnore>& render_ready_call = GX_DEFAULT_IGNORED_END_CALLER) noexcept;
    void start() noexcept;
};
}

#endif