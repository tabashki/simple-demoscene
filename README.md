Simple Demoscene
================

This repo contains a simple Win32 demoscene starter project that can be compiled and compressed down to an executable size of *less than 3 KiB* with the aid of the [Crinkler compressing linker](https://github.com/runestubbe/Crinkler).


Project Structure
-----------------

All the source code for the project is contained within the `/src/` directory, and can be compiled using the `simple-demoscene.sln` Visual Studio 2022 solution provided. External tools such as `crinkler` and `yasm` are provided in the `/extern/` directory, and have been setup to work out-of-the box with Visual Studio.


⚠️ Anti-Virus Warning ⚠️
-------------------------

A lot of virus developers use similar executable compression and packing tools to avoid detection from various anti-virus software. At some point one or more trojan devvelopers decided to use Crinkler to pack their malware, which has unfortunately caused the unpacking runtime shared between every Crinkled executable to be permanently flagged in the databases of various anti-virus vendors.

Basically, don't be surprised if your `Crinkled` demoscene executable is immediately flagged as malicious after compiling. Apart from adding manual exclusions, so far I haven't seen a better way around this false-positive.


Build Configurations
--------------------

### Debug
- No optimizations, ideal for development
- Includes asserts and extra cleanup code useful for catching bugs
- Uses the standard MSVC linker

### Release
- Full optimizations, as close as possible to the final compressed executable while still allowing for fairly quick iteration
- No asserts or cleanup code is compiled in
- Uses the standard MSVC linker

### Crinkled
- Same as `Release` but instead uses the `Crinkler` compressing linker
- Expect a long delay on startup as the executable unpacks itself


Rendering
---------

The demoscene uses Direct3D 11 for all rendering work. Currently everything is bundled into a single compute shader `cs_scene_render.hlsl` that writes into a swapchain backbuffer UAV each frame. Lighting is calculated using signed distance field path tracing, for which there are number of primitive shapes with intersection functions defined in `common.hlsli` for convenience.

Future Work
-----------

 - Sound & Music
 - Resolution selection on startup
 - A rudimentary scene system with timeline

Licenses
========

The source code in this repository is licensed under the [MIT License](/LICENSE.txt).

Crinkler is Copyright © 2005-2020 Aske Simon Christensen and Rune L. H. Stubbe, licensed under a [custom permissive license](/extern/crinkler/LICENSE.txt).

Yasm is Copyright © 2001-2014 Peter Johnson and other Yasm developers, [licensed under 2-clause BSD](/extern/yasm/LICENSE.txt).
