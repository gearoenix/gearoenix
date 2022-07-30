#include "gx-cr-sync-influence-manager.hpp"

gearoenix::core::Id gearoenix::core::sync::InfluenceManager::add(
    std::vector<Id> influenced_ids,
    const double priority,
    std::function<void()> function) noexcept
{
    std::lock_guard<std::mutex> _lg(actions_lock)
        const auto id
        = asset::Manager::create_id();
    actions.emplace_back(std::make_tuple(id, std::move(influenced_ids), priority, std::move(function)));
    return id;
}

void gearoenix::core::sync::InfluenceManager::remove(const Id function_id) noexcept
{
    GX_GUARD_LOCK(actions)
    actions.emplace_back(function_id);
}

void gearoenix::core::sync::InfluenceManager::update() noexcept
{
    {
        GX_GUARD_LOCK(actions)
        if (actions.empty())
            return;
        for (auto& a : actions) {
            if (const auto id = std::get_if<Id>(&a)) {
                added_functions.erase(*id);
            } else {
                auto& t = std::get<std::tuple<Id, std::vector<Id>, double, std::function<void()>>>(a);
                added_functions.emplace(
                    std::get<0>(t),
                    std::make_tuple(std::move(std::get<1>(t)), std::get<2>(t), std::move(std::get<3>(t))));
            }
        }
    }
    Id influencer = 1;
    std::map<Id, Id> influenced_to_influencer;
    std::map<Id, std::pair<std::set<Id>, std::multimap<double, std::function<void()>>>> influencer_to_influenceds;
    for (auto& id_fun_info : added_functions) {
        const auto& fun_info = id_fun_info.second;
        const auto& inf = std::get<0>(fun_info);
        std::set<Id> found_influencers;
        for (const Id i : inf) {
            const auto search = influenced_to_influencer.find(i);
            if (search == influenced_to_influencer.end())
                continue;
            found_influencers.emplace(search->second);
        }
        if (found_influencers.size() == 1) {
            const auto found_influencer = *found_influencers.begin();
            auto& iti = influencer_to_influenceds[found_influencer];
            auto& influenceds = iti.first;
            for (const Id i : inf) {
                influenced_to_influencer[i] = found_influencer;
                influenceds.emplace(i);
            }
            iti.second.emplace(std::get<1>(fun_info), std::get<2>(fun_info));
        } else {
            auto& iti = influencer_to_influenceds[influencer];
            auto& influenceds = iti.first;
            auto& functions = iti.second;
            for (const Id i : inf) {
                influenced_to_influencer[i] = influencer;
                influenceds.emplace(i);
            }
            functions.emplace(std::get<1>(fun_info), std::get<2>(fun_info));
            for (const Id old_influencer : found_influencers) {
                auto& old_influencer_info = influencer_to_influenceds[old_influencer];
                auto& old_influenceds = old_influencer_info.first;
                for (const Id influenced : old_influenceds) {
                    influenced_to_influencer[influenced] = influencer;
                }
                influenceds.merge(old_influenceds);
                functions.merge(old_influencer_info.second);
                influencer_to_influenceds.erase(old_influencer);
            }
            ++influencer;
        }
    }

    influencers_indices.clear();
    influencers.clear();

    influencers_indices.reserve(added_functions.size());
    influencers.reserve(added_functions.size());

    for (auto& iti : influencer_to_influenceds) {
        auto& functions = iti.second.second;
        for (auto& pri_fun : functions) {
            influencers.emplace_back(std::move(pri_fun.second));
        }
        influencers_indices.emplace_back(influencers.size());
    }
}
