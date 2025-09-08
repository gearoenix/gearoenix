#pragma once
#include "gx-cr-sync-parallel-for.hpp"
#include "gx-cr-sync-thread.hpp"
#include <atomic>
#include <vector>

BOOST_AUTO_TEST_CASE(gearoenix_core_sync_parallel_for)
{
    for (std::size_t i = 0; i < 200; ++i) {
        std::vector<std::unique_ptr<std::atomic<int>>> seen(i);
        for (auto j = decltype(i) { 0 }; j < i; ++j) {
            seen[j] = std::make_unique<std::atomic<int>>(0);
        }
        gearoenix::core::sync::parallel_for(seen, [](auto& s, auto) { ++*s; });
        int index = 0;
        for (const auto& v : seen) {
            BOOST_CHECK(*v == 1);
            if (*v != 1) {
                GX_LOG_E("Index: " << index << " Value: " << *v);
            }
            ++index;
        }
    }
}