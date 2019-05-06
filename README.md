# Gearoenix Game Engine
Yet another cross-platform C++ 3D game engine.

[![codecov](https://codecov.io/gh/Hossein-Noroozpour/gearoenix/branch/master/graph/badge.svg)](https://codecov.io/gh/Hossein-Noroozpour/gearoenix)
[![CircleCI](https://circleci.com/gh/Hossein-Noroozpour/gearoenix.svg?style=svg)](https://circleci.com/gh/Hossein-Noroozpour/gearoenix)


## Features
- It is cross-platform.
- It is Lightweight.
- It has a runtime abtraction over Graphic APIs (e.g. `OpenGL ES2`, `OpenGL ES3(WIP)`, 
  `Directx11(WIP)` and `Vulkan(WIP)`).
- It has its own file format for importing scenes data in to game. This file
  format help the engine to have better performance in loading a scene and
  reduce the size of the data hence It make smaller publishable binaries.
- It has its own
  [blender file exporter](https://github.com/Hossein-Noroozpour/gearoenix-blender)
  that try to force best practices in data creation and have control
  over content creation.
- Asset management that cache assets for reducing load time and making the reuse of assets
  more efficient.
- Multithreaded rendering.
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
- Pbr Material.
- Ability to create customized and or special materials/effects with special behaviors.


## Demos
I'm developing gearoenix and a game with it, and I'm little on the urge, do not
expect a full functioning AAA game as a demo from me :D. These are mostly in
debug mode of code, but still have great performance.
- [This is WebAssembly demo](https://hossein-noroozpour.github.io/gearoenix-static-files/web-demo/index.html)
- [This is Android demo](https://hossein-noroozpour.github.io/gearoenix-static-files/android/android.7z)
- [This is Linux demo](https://hossein-noroozpour.github.io/gearoenix-static-files/linux/linux.7z)


## How to Build?
- Star this project (:D)
- Download zip of master branch.
- Execute the setup.py with your python3 compiler. (This script provide a simple way of gathering
  dependencies of project and organizing the work space.)
- Create a build directory and cd it. (e.g. 'build')
- Do camke on it. (like: ```cmake -DCMAKE_BUILD_TYPE=Release .```)
- Build it. (like: ```cmake --build . --config Release```)
- If you have a problem to initialize the environment, 
  you either can use the provided docker images in docker hub or
  look at the provided **Dockerfile** file in project ci part.


## Development Process
Design and decision making in development process of this project is dependent
on games that I'm developing. I design and develop a new feature whenever my
games need it, sometimes in the
design and or implementation I makes decisions that are mostly related to my
game and urge of getting results, but I never ever disregards best practices in
its development. Test coverage is very low, I will improve it and any contribution
is highly regarded.

## License
You can do whatever you want to do with it as long as you take responsibility
of all of
its consequences, **But** If you used it and it was useful for you, please make
an acknowledgment and promotion for this project and me, I'm currently seeking 
for a job in the graphic and game-engine fields.
