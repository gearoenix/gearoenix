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
