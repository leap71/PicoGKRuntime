# Welcome to PicoGK Runtime

PicoGK Runtime is the C++ backbone to [PicoGK](https://picogk.org) a compact, robust geometry kernel for [Computational Engineering](https://leap71.com/computationalengineering/). 

This repo is hosted at: https://github.com/leap71/PicoGKRuntime

You will usually not clone this directory, unless you want to compile the PicoGK Runtime yourself instead of [using the available installers.](https://github.com/leap71/PicoGK/releases)

PicoGK Runtime is based on the [OpenVDB](https://github.com/AcademySoftwareFoundation/openvdb) and [GLFW](https://github.com/glfw/glfw) open source projects.

For more information, [please see the main PicoGK repository](https://github.com/leap71/PicoGK).

## Building on Linux

The runtime builds on Linux x86-64 with GCC and the system packages for Boost, TBB and Blosc (tested on Ubuntu 26.04 with GCC 15.2 and CMake 4.2).

```sh
git clone --recursive https://github.com/leap71/PicoGKRuntime
cd PicoGKRuntime
sh Install_Dependencies/Linux.sh
cmake -S . -B Build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build Build
```

This produces `Build/lib/picogk.26.2.so` (also copied to `Dist/`), the name the PicoGK C# library loads. Place it next to your application, or in a directory listed in `LD_LIBRARY_PATH`. OpenVDB is linked statically; TBB, Blosc, Boost.Iostreams and zlib are loaded from the system.

PicoGK was developed by Lin Kayser, and is maintained by [LEAP 71](www.leap71.com). PicoGK and PicoGK Runtime are released under the [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0) (SPDX Apache-2.0).  