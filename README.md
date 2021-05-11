# Smorgasbord graphics library (Version NaN)

Smorgasbord is a C++ library for quick to setup, quick to modify real-time
graphics. It tries to provide a thin, concise layer over standard graphics
APIs (Vulkan, OpenGL, DirectX) with reasonable overhead abstractions.

In its current state, I would not recommend using this library. I expect
frequent API breaking changes until it becomes more feature complete.

Some key design choices:

 - Not overbearing: will not turn your app into a Smorgasbord app. It doesn't ask for your argc, argv.
 - Can be used on the side: can be integrated into an existing codebase
 - Shader source generation over parsing: massively simplifies dealing with shaders, interface changes can be implemented much quicker. Interface code generation results in a more concise codebase, and eliminates any need for shader reflection.
 - Declarative geometry specification: in my experience describing the geometry instead of selecting a drawing function is more intuitive and results in quicker code changes
 - Modern perspective, terminology: even OpengGL is abstracted through queues, command buffers and pipelines. It might be superfluous for OpenGL, but helps maintaining the proper mindset, and helps newcomers to familiarize with the modern terminology.
 - Transparent abstraction: you can always reach the underlying APIs if you need something that isn't covered, or more straightforward doing manually.

Shortcomings:

 - The current API does not promote ahead of time pipeline state preparation, because it somewhat opposes the quick to modify attitude. Unless I find way to reconcile the two behind the API, I will try to provide a way to gradually transition away from on demand pipeline state compilation.

## Compiling the source

This project uses the CMake build scripts.

You have to either install GLM, FMT, and SDL2 system wide or set CMAKE_PREFIX_PATH when you invoke CMake. 
The latter approach is also useful if you want to manually build the dependencies to match the build type (e.g. RelWithDebInfo or Debug)

The build scripts are tested under QtCreator/MinGW32 and VS2017/MSVC14.

## Contributions

I reserve the right to relicense the library for any purpose in its entirety,
with or without third-party contributions. Even if it means a closed source
and/or commercial license. Only contribute changes to the library if you're
OK with that.

## License

Copyright 2018 Gábor Könyvesi

This project is licensed under the MIT License
See the LICENSE file for licensing terms
