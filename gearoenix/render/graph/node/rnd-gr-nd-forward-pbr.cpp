#include "rnd-gr-nd-forward-pbr.hpp"
#include "../../rnd-engine.hpp"

gearoenix::render::graph::node::ForwardPbr::ForwardPbr(Engine * e): Node(e)
{
	frames.resize(2);
	frames.push_back(Frame(e));
	frames.push_back(Frame(e));
}

gearoenix::render::graph::node::ForwardPbr::~ForwardPbr()
{
}

gearoenix::render::graph::node::ForwardPbr::Frame::Frame(Engine *e)
{
}

gearoenix::render::graph::node::ForwardPbr::Frame::~Frame()
{
	delete primary_cmd;
	primary_cmd = nullptr;
	delete semaphore;
	semaphore = nullptr;
}

gearoenix::render::graph::node::ForwardPbr::Frame::Kernel::~Kernel()
{
	delete secondary_cmd;
	secondary_cmd = nullptr;
	for (std::tuple<buffer::Uniform *, pipeline::ResourceSet *> &data : render_data_pool) {
		delete std::get<0>(data);
		delete std::get<1>(data);
	}
}
