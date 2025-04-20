#pragma once

#include <boost/container/flat_map.hpp>
#include <boost/container/static_vector.hpp>

namespace gearoenix::core {

template <typename Key, typename Value, std::size_t Size, typename Compare = std::less<Key>>
using static_flat_map = boost::container::flat_map<Key, Value, Compare, boost::container::static_vector<std::pair<Key, Value>, Size>>;
}