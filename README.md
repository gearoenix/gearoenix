# Gearoenix Game Engine
A cross-platform c++ 3d game engine.

[![codecov](https://codecov.io/gh/Hossein-Noroozpour/gearoenix/branch/master/graph/badge.svg)](https://codecov.io/gh/Hossein-Noroozpour/gearoenix)
[![CircleCI](https://circleci.com/gh/Hossein-Noroozpour/gearoenix.svg?style=svg)](https://circleci.com/gh/Hossein-Noroozpour/gearoenix)

## Features
- It is cross-platform (already tested on Linux(Ubuntu, Fedora), MacOS,
  Windows, Android, iOS and WebAssembly)
- It has `OpenGL ES2`, `OpenGL ES3`, `Directx11` and `Vulkan(WIP)` backends
  but also it is ready to have `Metal` in future (because, its design is
  based on my previous game engine,
  [Vulkust](https://github.com/Hossein-Noroozpour/vulkust)).
- It has its own file format for importing scenes data in to game. This file
  format help the engine to have better performance in loading a scene and
  reduce the size of the data.
- It has its own
  [blender file exporter](https://github.com/Hossein-Noroozpour/gearoenix-blender)
  that try to force best practices in data creation and have control
  over content creation.
- Asset management that cache assets for reducing load time.
- Supports variety of textures:
  - 2D texture
  - 3D texture (WIP, Implemented in blender part, but it is not developed in
    engine yet)
  - Cube texture
- Painless cross-platform mathematic structures.
- Lighting
- Shadowing
- Skyboxing
- Fast font displaying mechanism.
- Different type of cameras: Perspective, Orthographic
- Graphic pipeline management to cache and reduce number of switches and
  increasing performance.
- Multithreaded occlusion culling.
- Multithreaded mathematic works for increasing performance.
- Variety of materials:
  - Lighting: Shadeless / Directional / Normal-mapped
  - Texturing: Colored / 2D / 3D(WIP) / Cube
  - Speculating: Speculated / Speculare-texture / Not-speculated
  - Shadowing: Caster / Receiver-Caster / No-shadow
  - Reflection: Realtime(WIP) / Baked / Nonreflective
  - Transparency: Translucent / Cutoff / Opaque
  - A way to create customized and or strange(!!) materials with special
    behaviors.
  - (Note: All of the materials are developed in blender-plugin but few number
    of materials are not developed in engine. If you want any of them you can
    contact me or very easily implement them yourself.)


## Demos
I'm developing gearoenix and a game with it, and I'm little on the urge, do not
expect a full functioning AAA game as a demo from me :D. These are mostly in
debug mode of code, but still have great performance.
- [This is WebAssembly demo](https://hossein-noroozpour.github.io/gearoenix-static-files/web-demo/index.html)
- [This is Android demo](https://hossein-noroozpour.github.io/gearoenix-static-files/android/android.7z)
- [This is Linux demo](https://hossein-noroozpour.github.io/gearoenix-static-files/linux/linux.7z)

## How to Build?
- Star this project (:D)
- Git clone this repository
- Fetch its sub-modules.
- This game engine, itself can easily added to your project and compile along
  your game code but In some platforms it needs dependencies that you must
  provide.
- There is a rule that apply in overall of the engine:
  - I always use the latest available technologies, so If you want to use it,
    you have to be as update as possible.
- For MacOS, iOS and Linux you need SDL2.
  - For MacOS and iOS, you need a little knowledge of SDL2 compilation for
    these platforms and also I have put a helper script (`ios-build.sh`),
    this can help you a little bit.
  - In Linux you have an easier path, only install SDL2 developer package from
    your distributor.
- In Linux in in addition to SDL2 you needs `clang++`.
- In MacOS you needs XCode.
- In Windows you need Visual Studio and full functioning Directx11 SDK.
- For Android, Android Studio and NDK.
- Run it!
- There are some build scripts like `linux-build.sh` it helps you to find
  your path.
- However, If you have problem with its build you can contact me.

## Design pattern
- Code design and design patterns of the project are based on modern C++.
- Design patterns of Gearoenix are influenced by lots several other design
  patterns, but
  it has its own patterns so If you want to make a change in it you must study
  its code (and that is really easy :D).
- In most of the time, I try to not use modern features of C++,
  unless those feature add a great
  **design, performance, maintainability**
  value to my work
  because most of the new features have overhead like `shared_ptr` (runtime
  overhead) or `auto` (compile time overhead), but actually I'm using a lot of
  modern C++ stuff in Gearoenix.
- Compile time is important because I run the project even after a small
  change and engine gonna be used with its source code.
- I'm using `clang-format` with style of `WebKit` to rule all of the format
  stuffs, the `push.sh` do this and it only run in my Linux desktop at home
  (because of changes in different versions of `clang-format`).
- Gearoenix has a monomorphism interface over system application.
  This interface implemented for:
  SDL2, Android NDK and Windows API.
- There is a polymorphism interface over render engine. For example in runtime
  based on the machine that application is running on, Gearoenix can decide to
  use which render engine. Current implementations are:
  GLES2, GL3, Directx11 and Vulkan(WIP)
- More is todo.

## Development Process
Design and decision making in development process of this project is dependent
on a game that I'm developing. I design and develop a new feature whenever my
game needs that new feature in the engine, sometimes in the
design and or implementation I makes decisions that are mostly related to my
game and urge of getting results, but I never ever disregards best practices in
its development.

One of the appearance of this development process in engine is, at very start,
this engine was getting developed with `Vulkan API` but because of lake of
market share and slow development, I decided to suspend `Vulkan` part and
continue with `OpenGL ES 2/3` and `Directx11` (for Windows).

## TODOs
Sorted in order of importance.
- Physics:
    - Collision detection 50% (blender part is down)
    - Rigid body 00%
- 3D Audio (00%)
- Vulkan (WIP 40%)
- Metal (00%)

## License
You can do whatever you want to do with it as long as you take responsibility
of all of
its consequences, **But** If you used it and it was useful for you, please make
an acknowledgment and promotion for this project and me, I'm really need that,
because I'm currently seeking for a job in the graphic and game-engine fields.
