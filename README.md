<img align="left" alt="" src="https://github.com/Hossein-Noroozpour/gearoenix-static-files/raw/master/logo.png" height="150" />

# Gearoenix Game Engine

A cross-platform C++ 3D game engine.

[![codecov](https://codecov.io/gh/Hossein-Noroozpour/gearoenix/branch/master/graph/badge.svg)](https://codecov.io/gh/Hossein-Noroozpour/gearoenix)
[![CircleCI](https://circleci.com/gh/Hossein-Noroozpour/gearoenix.svg?style=svg)](https://circleci.com/gh/Hossein-Noroozpour/gearoenix)

<br>
<br>

## Status

Finalizing 0.2 APIs, in case you used it, be patient about API changes, soon
it's going to be stabilized.

## Features

- It is cross-platform.
- It is Lightweight.
- Highly multithreaded architecture.
- It has a runtime abstraction over Graphic API backends (e.g. `OpenGL ES2`, `OpenGL ES3`,
  `OpenGL 3.3`, `OpenGL 4.3`, `Directx11(WIP)` and `Vulkan(WIP)`).
- It can automatically choose the highest available backend and in addition in your build you can
  off an API backend so it will not compile that API and then your binary size will be reduced.
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
  - 3D texture (WIP, it has been implemented in blender part, but it is not developed in
    engine yet)
  - Cube texture
- Painlessly cross-platform and very fast mathematics structures.
- Lighting
- Shadowing
- Skyboxing (Cube and Equirectangular)
- Fast font displaying mechanism.
- Different type of cameras: Perspective, Orthographic
- Graphic pipeline management to cache and reduce number of switches and
  increasing performance.
- Multithreaded occlusion culling.
- Multithreaded mathematics works for increasing performance.
- PBR Material.
- Ability to create customized and or special materials/effects with special behaviors.
- Automatically Adaptive cascaded shadow mapping based on the scene and camera properties.
- Runtime reflection probe with smooth rendering which divides rendering process between frames.
- A GUI and CLI tool for baking HDRI to environment, irradiance and radiance cube-maps.

## Demos

In this section I need contribution. (Note: Webassembly is not main target of this project.)

- [This is WebAssembly demo (Firefox shows better)](https://hossein-noroozpour.github.io/gearoenix-static-files/web-demo/GearoenixDemoWorldWideRacing.html)

## How to Build

- Star this project (:D)
- Download ZIP of master branch.
- Execute the `setup.py` with your Python3 compiler. (This script provide a simple way of gathering
  dependencies of project and organizing the work space.)

### Desktop OSs

- Create a build directory and `cd` it. (e.g. 'build')
- Do Cmake on it. (like: ```cmake -DCMAKE_BUILD_TYPE=Release ..```)
- Build it. (like: ```cmake --build . --config Release```)
- If you have a problem to initialize the environment,
  you either can use the provided Docker images in Docker-hub or
  look at the provided **Dockerfile** file in project ci part.

### Android

- You must have the latest version of android studio, SDK and NDK installed
- Open the android folder with android studio.
- Run and enjoy.

### iOS

- You must have the latest XCode and iOS SDK installed
- Create a build directory and `cd` it. (e.g. 'build')
- Do Cmake on it. (like: ```cmake -DCMAKE_BUILD_TYPE=Release .. -DCMAKE_SYSTEM_NAME=iOS```)
- Open the XCode project.
- Connect your iOS device (currently there is a bug in one of the static library of ios-simulator that prevents the build for it)
- Sign the project with your ios-developer team
- Run and enjoy.

### Webassembly

- Create `assets` directory in the root of `Gearoenix` project.
- Place your (or example's) required assets files in `assets` directory.
- Create a build directory (e.g. `wasm-build`) and `cd` it.
- You must have Emscripten SDK installed.
- Your console must have Emscripten SDK in its environment.
- ```emcmake cmake -DCMAKE_BUILD_TYPE=Release ..```
- ```cmake --build . --config Release```
- Create a web-server for the application, for example, like following: 
  - If you have Python3 in your path do ```python3 -m http.server 8080```
  - For Python2 ```python -m SimpleHTTPServer 8080```
- Open your browser and go to ```http://localhost:8080/examples/<name-of-example>```

## Supporters & Sponsors

<a href="https://www.jetbrains.com/?from=Gearoenix">
  <img src="https://github.com/Hossein-Noroozpour/gearoenix-static-files/raw/master/supporters/jetbrains-variant-3.svg" width="201px" alt="jetbrains">
</a>

## Contribution

- Donating to this project.
- Promotion of this project.
- Documentation.
- Test coverage is not very well and I don't have time for it,
  if you want to contribute in it this is the best place you can start.
- Documentation.
- 3D content development.

## License

You can do whatever you want to do with it as long as you take responsibility
of all of
its consequences, **But** If you used it and it was useful for you, please make
an acknowledgment and promotion for this project and me, I'm currently seeking
for a job in the graphic and game-engine fields.
