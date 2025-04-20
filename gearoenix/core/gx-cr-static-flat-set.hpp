#pragma once

#include <boost/container/flat_set.hpp>
#include <boost/container/static_vector.hpp>

namespace gearoenix::core {

template <typename Element, std::size_t Size, typename Compare = std::less<Element>>
using static_flat_set = boost::container::flat_set<Element, Compare, boost::container::static_vector<Element, Size>>;
}