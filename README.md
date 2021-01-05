# Gearoenix (vulkan-backend)

Vulkan backend of Gearoenix game engine.

![Ubuntu 20.04 release build](https://github.com/Hossein-Noroozpour/gearoenix-vulkan-ray-tracing/workflows/Ubuntu%2020.04%20release%20build/badge.svg)


## Why a separate repository?

Gearoenix at the start was heavily designed by the properties of
Vulkan and other low level APIs but after a while the need of supporting
poor hardware raised and decision changed to support old APIs like
OpenGL-ES2 and after that most of the implementations happened in
those part and Vulkan part of the Gearoenix dropped behind of the
OpenGL part.
But start of the design was based on Vulkan and it remained as the
main criteria of design of the new features.
Right now Vulkan part of the Gearoenix can not compile and it has
a long road to become compatible with new changes in engine and to
reach something integratable and testable and also there are several
decisions to change in some part of the design of Gearoenix.
After the promising results from this project has been shown, it is
going to be integrated to Gearoenix.


## Decisions

- Right now a standalone application is going to be implemented, not a library.
- The only advantage of Vulkan over OpenGL (modern versions) for Gearoenix is
  better multithreaded rendering and ray tracing
- GLTF file is going to be used as the main asset provider format instead of
  GX3D file of the Gearoenix, it provides better mechanism for testing and
  speeds the implementation process.
- <s>Instead of GC structure of the Gearoenix, VulkanMemoryAllocator is going
  to be used and after several benchmarking maybe GC structure is going to be
  used</s> The Gearoenix own allocator structure is going to be used as the memory manager and allocator.
- CI/CD will always look for newest dependencies to keep the project as update as possible.