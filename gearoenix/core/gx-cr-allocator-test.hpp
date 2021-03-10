#ifndef GEAROENIX_CORE_ALLOCATOR_TEST_HPP
#define GEAROENIX_CORE_ALLOCATOR_TEST_HPP
#include "gx-cr-allocator.hpp"

BOOST_AUTO_TEST_CASE(gearoenix_core_allocator)
{
    auto root = gearoenix::core::Allocator::construct(1000);
    BOOST_TEST(root->get_size() == 1000);
    BOOST_TEST(root->get_offset() == 0);

    auto a1 = root->allocate(100);
    BOOST_TEST(a1->get_size() == 100);
    BOOST_TEST(a1->get_offset() == 0);

    auto a2 = root->allocate(100);
    BOOST_TEST(a2->get_size() == 100);
    BOOST_TEST(a2->get_offset() == 100);

    auto a3 = root->allocate(100);
    BOOST_TEST(a3->get_size() == 100);
    BOOST_TEST(a3->get_offset() == 200);

    auto a4 = root->allocate(100);
    BOOST_TEST(a4->get_size() == 100);
    BOOST_TEST(a4->get_offset() == 300);

    auto a5 = root->allocate(100);
    BOOST_TEST(a5->get_size() == 100);
    BOOST_TEST(a5->get_offset() == 400);

    auto a6 = root->allocate(500);
    BOOST_TEST(a6->get_size() == 500);
    BOOST_TEST(a6->get_offset() == 500);

    a2 = a3 = a4 = a5 = nullptr;

    a2 = root->allocate(300);
    BOOST_TEST(a2->get_size() == 300);
    BOOST_TEST(a2->get_offset() == 100);

    a3 = a2->allocate(100);
    BOOST_TEST(a3->get_size() == 100);
    BOOST_TEST(a3->get_offset() == 100);

    a4 = a2->allocate(200);
    BOOST_TEST(a4->get_size() == 200);
    BOOST_TEST(a4->get_offset() == 200);

    a5 = a2->allocate(100);
    BOOST_TEST(nullptr == a5);
}

#endif