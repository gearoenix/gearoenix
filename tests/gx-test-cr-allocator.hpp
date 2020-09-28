#ifndef GEAROENIX_TEST_CORE_ALLOCATOR_HPP
#define GEAROENIX_TEST_CORE_ALLOCATOR_HPP
#include <gearoenix/core/gx-cr-allocator.hpp>

BOOST_AUTO_TEST_CASE(gearoenix_core_allocator_test)
{
    auto alc = gearoenix::core::Allocator::construct(100);
    auto a1 = alc->allocate(50);
    BOOST_TEST(0 == a1->get_offset());
    auto a2 = alc->allocate(50);
    BOOST_TEST(50 == a2->get_offset());
    a1 = nullptr;
    auto a3 = alc->allocate(50);
    BOOST_TEST(0 == a3->get_offset());
    a2 = nullptr;
    auto a4 = alc->allocate(20);
    BOOST_TEST(50 == a4->get_offset());
    auto a5 = alc->allocate(20);
    BOOST_TEST(70 == a5->get_offset());
    a4 = nullptr;
    auto a6 = alc->allocate(5);
    BOOST_TEST(90 == a6->get_offset());
    a5 = nullptr;
}
#endif