# Smorgasbord rendering library (Version NaN)

Smorgasbord is a real-time rendering library aiming to aid experimentation
and helps implementing rendering pipelines in a concise way without being
vague about the underlying operations.

In its current state, I would not recommend using this library. I expect
frequent API breaking changes until it becomes more feature complete.

## Compiling the source

The library needs the "dep_src" repository contents to compile. Set the
DEP_SRC_DIRECTORY cmake option or clone the two repos next to each other.

The cmake scripts currently assume a QtCreator MinGW32 environment. You will
need to tweak these scripts if you want an other configuration (or wait until
I get to it).

## Contributions

I reserve the right to relicense the library for any purpose in its entirety,
with or without third-party contributions. Even if it means a closed source
and/or commercial license. Only contribute changes to the library if you're
OK with that.

## License

Copyright 2018 Gábor Könyvesi

See the LICENSE file for licensing terms
