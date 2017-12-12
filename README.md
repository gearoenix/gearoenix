# Gearoenix Game Engine
An in-house game engine with experimental approach.

## Features
- It is cross-platform (already tested on Linux, Android, iOS and WebAssembly
  but, It can easily compile for Windows, MacOS, ...)
- It has `OpenGL` and `Vulkan(WIP)` backends but also it is ready to have
  `Metal` in future (because of its nature of being successor of Vulkust game
  engine).
- It has its own file format for importing scenes data in to game. This file
  format help the engine to have better performance in loading a scene and
  reduce the size of the data.
- It has its own blender file exporter that try to force best practices in data
  creation and have control over content creation.
- Asset management that cache assets for reducing load time.
- Supports variety of textures:
  - 2D texture
  - 3D texture (Implemented in blender part, but it is not developed in engine
    yet)
  - Cube texture
- Painless cross-platform mathematic structures.
- Lighting
- Shadowing
- Different type of cameras: Perspective, Orthographic
- Graphic pipeline management to cache and reduce number of switches and
  increasing performance.
- Multithreaded occlusion culling.
- Variety of materials:
  - Lighting: Shadeless / Directional / Normal-mapped
  - Texturing: Colored / 2D / 3D / Cube
  - Speculating: Speculated / Speculare-texture / Not-speculated
  - Shadowing: Caster / Receiver-Caster / No-shadow
  - Reflection: Realtime / Baked / Nonreflective
  - Transparency: Translucent / Cutoff / Opaque
  - A way to create customized and or strange(!!) materials with special
    behaviors.
  - (Note: All of the materials are developed in blender-plugin but few number
    of materials are not developed in engine. If you want any of them you can
    contact me or very easily implement them.)


## Demos
I'm developing gearoenix and a game with it, and I'm little on the urge, do not
expect a full functioning AAA game from me :D.
- [This is WebAssembly demo of game.](https://hossein-noroozpour.github.io/gearoenix-static-files/web-demo/index.html)
- [This is Android one.](https://hossein-noroozpour.github.io/gearoenix-static-files/android/android.7z)
- [This is Linux one.](https://hossein-noroozpour.github.io/gearoenix-static-files/linux/linux.7z)

# How to Build?

- Linux:
  - Star this project (:D)
  - Git clone this repository
  - Install sdl2 development package in your Linux distribution.
  - Open your lovely IDE, I personally use Qt Creator.
  - Add all the gearoenix folder to your project.
  - Add SDL2 and GL and pthread support to your compiler settings.
  - If you are building the default test of gearoenix you need a static file
    download it from
    [here](https://hossein-noroozpour.github.io/gearoenix-static-files/data.gx3d)
    and place it in the directory of executable.
  - Run it!


In other platform, the build process is a little complicated, because first I
don't have time to provide tools for it, second, gearoenix is changing a lot and
third, it is an in-house game engine. But there are several helper scripts,
like mac-build.sh and webassembly/build.sh but still you can not create a
useful thing with it, because game engine have specific file format that can
be created with my blender exporter plugin. If you desperately want to build it,
you can contact me with email. hossein.noroozpour@gmail.com

## Development Process
Design and decision making in development process of this project is business
oriented, it means: I'm developing a game with it and development of this
project is dependent on the needs of that game and I design and develop a new
feature whenever my game needs that new feature in the engine, sometimes in the
design and or implementation I makes decisions that are mostly related to my
game and urge of getting results, but I never ever disregards best practices in
its development.

One of the appearance of this development process in engine is, at very start,
this engine was getting developed with `Vulkan API` but because of lake of
market share and slow development, I decided to suspend `Vulkan` part and
continue with `OpenGL ES 2`.

## TODOs
Sorted in order of importance.
- Direct3D 11 (just started, because I saw the OpenGLES-X is not welcome
  anymore in Windows).
- Physics:
    - Collision detection 50% (blender part is down)
    - Rigid body 00%
- Vulkan (WIP 40%)


## License
You can do whatever you want to do as long as you take responsibility of all of
its consequences, **But** If you used it and it was useful for you, please make
an acknowledgment and promotion for this project and me, I'm really need that
because I'm currently seeking for a job in the graphic and game-engine fields.
