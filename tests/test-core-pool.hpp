#include <gearoenix/core/cr-pool.hpp>
#include <gearoenix/system/sys-log.hpp>

BOOST_AUTO_TEST_CASE(math_core_pool)
{
    gearoenix::core::OneLoopPool<int> pool;

    for (int i = 0; i < 10; ++i) {
        *pool.get_next([] { return new int; }) = i;
    }

    int i = 0;
    for (int& o : pool) {
        GXLOGD("Pool: " << o << ", Expectation: " << i)
        BOOST_CHECK_EQUAL(o, i);
        ++i;
    }

    i = 0;
    for (int& o : pool) {
        GXLOGD("Pool: " << o << ", Expectation: " << i)
        BOOST_CHECK_EQUAL(o, i);
        ++i;
    }

    BOOST_CHECK_EQUAL(pool.size(), 10);
    BOOST_CHECK_EQUAL(pool.capacity(), 10);

    pool.refresh();

    BOOST_CHECK_EQUAL(pool.size(), 0);
    BOOST_CHECK_EQUAL(pool.capacity(), 10);

    pool.set_increase_rate(5);

    for (i = 0; i < 10; ++i) {
        *pool.get_next([] { return new int; }) = i;
    }

    BOOST_CHECK_EQUAL(pool.size(), 10);
    BOOST_CHECK_EQUAL(pool.capacity(), 10);

    *pool.get_next([] { return new int; }) = 10;

    BOOST_CHECK_EQUAL(pool.size(), 11);
    BOOST_CHECK_EQUAL(pool.capacity(), 15);

    i = 0;
    for (int& o : pool) {
        GXLOGD("Pool: " << o << ", Expectation: " << i)
        BOOST_CHECK_EQUAL(o, i);
        ++i;
    }
    BOOST_CHECK_EQUAL(i, 11);
}