#include "gx-phs-anm-bone.hpp"
#include "../../core/macro/gx-cr-mcr-assert.hpp"

gearoenix::physics::animation::Bone::Index gearoenix::physics::animation::Bone::construct(
	std::vector<std::uint8_t>& memory,
	const std::string& name,
	const Index children_count) noexcept
{
	const auto result = static_cast<Index>(memory.size());
	memory.resize(memory.size() + sizeof(Bone) + sizeof(Index) * static_cast<std::size_t>(children_count));
	auto& bone = *new (&memory[result]) Bone();
	bone.name = name;
	bone.children_count = children_count;
	const auto last_ptr = static_cast<Index>(memory.size());
	for (Index ptr = result + sizeof(Bone); ptr < last_ptr; ptr += sizeof(Index))
	{
		*reinterpret_cast<Index*>(&memory[ptr]) = static_cast<Index>(-1);
	}
	return result;
}

void gearoenix::physics::animation::Bone::set_children(std::vector<std::uint8_t>& memory, const Index this_index, const std::vector<Index>& children) noexcept
{
	auto& bone = *reinterpret_cast<Bone*>(&memory[this_index]);
	GX_ASSERT_D(bone.children_count == static_cast<Index>(children.size()));
	Index children_ptr = this_index + sizeof(Bone);
	for (const auto child_ptr : children)
	{
		GX_ASSERT_D(*reinterpret_cast<Index*>(&memory[children_ptr]) == static_cast<Index>(-1)); // It is one off function
		*reinterpret_cast<Index*>(&memory[children_ptr]) = child_ptr;
		reinterpret_cast<Bone*>(&memory[child_ptr])->parent_index = this_index;
		children_ptr += sizeof(Index);
	}
}

gearoenix::physics::Transformation& gearoenix::physics::animation::Bone::get_transformation(std::vector<std::uint8_t>& memory, const Index this_index) noexcept
{
	return reinterpret_cast<Bone*>(&memory[this_index])->transform;
}

gearoenix::physics::animation::Bone::~Bone() noexcept
{
	GX_ASSERT_D(children_count == 0);
}

void gearoenix::physics::animation::Bone::update() noexcept
{
	transform.update();
	transform.clear();
}
