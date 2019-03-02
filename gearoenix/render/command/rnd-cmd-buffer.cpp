#include "rnd-cmd-buffer.hpp"

void gearoenix::render::command::Buffer::record(const std::shared_ptr<Buffer> &o) {
	recored_secondaries.push_back(o);
}