#include "rnd-buf-static.hpp"

gearoenix::render::buffer::Static::Static(const unsigned int size, const std::shared_ptr<engine::Engine>& e) : Buffer(size, e) {}

gearoenix::render::buffer::Static::~Static() {}