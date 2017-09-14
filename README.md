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

| Platforms             | Progress |
| --------------------- |:--------:|
| Linux (Ubuntu, Fedora)| 60%      |
| Android               | 60%      |
| Windows (Vulkan)      | 60%      |
| Macos (Vulkan)        | 00%      |
| Macos (Metal)         | 00%      |
| iOS (Metal)           | 00%      |
| Windows (DirectX12)   | 00%      |

## Design principles in code
- Gearoenix uses new and modern features in C++ in conditions that it does not
create any contrast and conflict with any of the goals in the project missions.
- Most of the time old C features are better to choose because maybe in future
some of the planned platforms may not support newer features of C++, but
if a particular feature in a particular place can create a huge advantage,
Geareonix never going to sacrifice that advantage.
