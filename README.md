<img src="https://github.com/Hossein-Noroozpour/gearoenix-static-files/raw/master/logo.png" width="100%"  alt=""/>

# Gearoenix

Gearoenix game engine.

[![Ubuntu-24.04  ](https://github.com/gearoenix/gearoenix/actions/workflows/linux.yml/badge.svg)](https://github.com/gearoenix/gearoenix/actions/workflows/linux.yml)
[![Windows-2022  ](https://github.com/gearoenix/gearoenix/actions/workflows/windows.yml/badge.svg)](https://github.com/gearoenix/gearoenix/actions/workflows/windows.yml)
[![WASM  ](https://github.com/gearoenix/gearoenix/actions/workflows/wasm.yml/badge.svg)](https://github.com/gearoenix/gearoenix/actions/workflows/wasm.yml)
[![codecov](https://codecov.io/gh/gearoenix/gearoenix/branch/master/graph/badge.svg?token=48NJY46U15)](https://codecov.io/gh/gearoenix/gearoenix)
[![Discord](https://img.shields.io/discord/700169912605474856.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/uGs66W6AM3)

## Examples

The `examples/` folder contains code demonstrating Gearoenix's functionalities. 

**WebAssembly Demos** (Note: Web data is not optimized for performance)

These demos showcase Gearoenix running in WebAssembly, but keep in mind that web data is not specifically optimized for this purpose.


[Example 004 Dear ImGui Usage](https://gearoenix.com/engine/webassembly-samples/005-hello-imgui/GearoenixExample005HelloImGui.html)

[Example 008 Baked Reflection Probes](https://gearoenix.com/engine/webassembly-samples/009-baked-reflection-probe/GearoenixExample009BakedReflectionProbe.html)

[Example 009 Lighting](https://gearoenix.com/engine/webassembly-samples/010-lighting/GearoenixExample010Lighting.html)

## Features

### Core Engine Features:

**🚀 High-Performance Multithreaded ECS:** Experience blazing-fast performance with our Entity Component System,
meticulously engineered for multithreaded execution.

**⚙️ Runtime Multi-Backend Graphics API Support:** Seamlessly deploy across platforms with support for OpenGL, Vulkan,
Metal, and DirectX, ensuring optimal rendering on any device.

**🎨 Versatile Shading Modes:** Achieve stunning visuals with a range of shading options, including PBR, Unlit,
Transparent, and more.

**📦 glTF/glb Importer:** Streamline your asset pipeline with native support for the industry-standard glTF and glb 
formats.

**🐞 Robust Debugging Tools:** Identify and resolve issues quickly with our comprehensive suite of validation checks and
debugging tools.

### Development Environment:

**🛠️ First-Class Dear ImGui Integration:** Leverage the power and flexibility of Dear ImGui for all your UI needs.

**🎛️ Built-in UI Widget System:** Create custom user interfaces with our integrated widget system, independent of Dear
ImGui.

**💻 Early-Stage Engine Editor:** Begin creating and refining your worlds with our integrated editor, designed to grow
with your project.

## Animation & Physics:

**💃 Armature Animation:** Bring your characters to life with smooth and realistic armature-based animation.

**✨ Sprite Animation:** Easily create dynamic 2D experiences with our dedicated sprite animation system.

**⚙️ Early-Stage Physics Engine:** Simulate realistic interactions with our performant physics engine, featuring
colliders, animation integration, and constraints.

### Rendering Capabilities:

**💡 Deferred and Forward Rendering:** Choose the rendering path that best suits your project's needs, with support for
both deferred and forward rendering.

**✨ Rendering Abstraction:** Future-proof your projects with our rendering abstraction layer, supporting both
rasterization and ray tracing render modes.

## Build Process

 - Have vcpkg installed and have `VCPKG_ROOT` environment variable set.
 - `git clone --recursive https://github.com/gearoenix/gearoenix.git`
 - `cd gearoenix`
 - `cmake --preset windows-release .`
 - `cmake --build --preset windows-release .`

## Licensing

The engine is free to use, **but**:
 - You can't claim you have written the code.
 - Any change in the code must be re-stated.
 - If you ever used this project, you ought to acknowledge this project and its main contributors.