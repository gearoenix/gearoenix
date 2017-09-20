# Gearoenix game engine
C++ game engine with modern low level APIs like: Vulkan, Metal, Directx12. This
game engine is continuer of the
[Vulkust](https://github.com/Hossein-Noroozpour/vulkust). (Currently this
repository is under lots of changes.)

## Missions
1. Performance
2. Small and maintainable code
3. Small executing binary file generation
4. Cross-platform
5. Fast development

## Progress
Measurements are based on the current reaching milestone, not total road-map.

Current focus of the project is on the Vulkan backend of engine and mainly in
Linux platform, I want to reach to a demo and a good structure in project.

#### Currently planned

| Platforms             | Progress |
| --------------------- |:--------:|
| Linux (Ubuntu, Fedora)| 70%      |
| Android               | 60%      |
| Windows (Vulkan)      | 60%      |

#### Near future plans
[Vulkust](https://github.com/Hossein-Noroozpour/vulkust) had a basic Metal
backend I think I can do it in near future for Gearoenix too.

| Platforms             | Progress |
| --------------------- |:--------:|
| Macos (Vulkan)        | 00%      |
| iOS (Vulkan)          | 00%      |
| Macos (Metal)         | 00%      |
| iOS (Metal)           | 00%      |

#### Not seriously planned
Lots of studies have done for Directx12 but right now DirectX12 and XBox is not
my target market so I will develop it in far future.

| Platforms             | Progress |
| --------------------- |:--------:|
| Windows (DirectX12)   | 00%      |
| Webassembly & WebGL   | 00%      |
| VR                    | 00%      |
| Playstation           | 00%      |
| XBox                  | 00%      |

## Design principles in code
- Gearoenix uses new and modern features in C++ in conditions that it does not
create any contrast and conflict with any of the goals in the project missions.
- Most of the time old C features are better to choose because maybe in future
some of the planned platforms may not support newer features of C++, but
if a particular feature in a particular place can create a huge advantage,
Geareonix never going to sacrifice that advantage.
- Compile time is important for me, it can buy lots of time in process of
testing and debugging, I'm not gonna sacrifice that with some futile modern C++
features, like `auto` (it increases compile time and in addition it can
sometimes cause some unexpected results and It can slow down (or sometimes stop)
code intelligences of the IDEs), so I'm try to not use them most of the
times.

## GPU Memory management

GPU APIs (like Vulkan, Metal, ...) lake lots of necessary features for memory
management (e.g. `memmove`, etc) so writing a full featured memory manager in
these APIs is sometimes very costly and in most of the cases can even have
several performance penalties. In Gearoenix, I tried to develop a customized
memory manager that does not do some of the performance costing stuffs that
other available memory allocators do, like memory consolidation, collocation
and expansion.
The Idea behind Gearoenix GPU memory management is:
- Performance of allocating and deallocating.
- Minimizing any external or internal synchronization cost.
- Removing memory waste in auxiliary memory in RAM.
- Trade off between fragmentation and performance.

It adds some responsibilities on the hands of game developers:
- It puts deallocation in the hands of the user of allocated memory instead
of doing deallocation via reference counting or other stuffs.
- If a game developer wants to use VRAM he/she must specify amount of the memory
he/she is going to use, because Gearoenix does not do any expansion in VRAM.

(Disclaimer: this allocator is not the best general purpose solution but it is good enough for the use of Gearoenix.)


## License
You can do whatever you want to do as long as you take responsibility of all of
its consequences, **But** If you used it and it was useful for you, please make
an acknowledgment and promotion for this project and me, I'm really need that
because I'm currently seeking for a job in the graphic and game-engine fields.
