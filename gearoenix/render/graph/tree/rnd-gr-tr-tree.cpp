#include "rnd-gr-tr-tree.hpp"
#include <utility>

gearoenix::render::graph::tree::Tree::Tree(std::shared_ptr<engine::Engine> e) noexcept
    : e(std::move(e))
{
}
