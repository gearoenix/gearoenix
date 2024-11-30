#pragma once
#include <functional>
#include <map>
#include <optional>
#include <set>
#include <variant>
#include <vector>

namespace gearoenix::core::sync {
struct InfluenceManager {
    GX_GET_CREF_PRV(std::vector<std::function<void()>>, influencers);
    GX_GET_CREF_PRV(std::vector<std::uint32_t>, influencers_indices);

private:
    std::map<Id, std::tuple<std::vector<Id>, double, std::function<void()>>> added_functions;
    std::mutex actions_lock;
    std::vector<std::variant<Id, std::tuple<Id, std::vector<Id>, double, std::function<void()>>>> actions;

public:
    /// \return The Id of the function (not the influencer id), in future a function can delete itself by it.
    [[nodiscard]] Id add(std::vector<Id> influenced_ids, double priority, std::function<void()> function);
    void remove(Id function_id);
    void update();
};
}