#ifndef GEAROENIX_TEST_CORE_SYNC_INFLUENCE_MANAGER_HPP
#define GEAROENIX_TEST_CORE_SYNC_INFLUENCE_MANAGER_HPP

#include "../gearoenix/core/sync/gx-cr-sync-influence-manager.hpp"

BOOST_AUTO_TEST_CASE(gearoenix_core_sync_influence_manager_test)
{
    gearoenix::core::sync::InfluenceManager inf_mgr;
    (void)inf_mgr.add({ 1, 2, 3 }, 1.0, [] {});
    (void)inf_mgr.add({ 3, 4, 5 }, 2.0, [] {});
    (void)inf_mgr.add({ 6, 7, 8 }, 3.0, [] {});
    const auto fid1 = inf_mgr.add({ 2, 4, 6 }, 4.0, [] {});

    inf_mgr.update();
    BOOST_CHECK(inf_mgr.get_influencers_indices()[0] == 4);

    inf_mgr.remove(fid1);
    inf_mgr.update();
    BOOST_CHECK(inf_mgr.get_influencers_indices()[0] == 2);
    BOOST_CHECK(inf_mgr.get_influencers_indices()[1] == 3);
}

#endif