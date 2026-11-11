# PicoGK Runtime

PicoGK Runtime is the native C++ backbone of [PicoGK](https://picogk.org/), a compact geometry kernel for [Computational Engineering](https://leap71.com/computationalengineering/), developed by [LEAP 71](https://leap71.com/).

This repository contains the C++ implementation and native interface of **PicoGK 2.x**, which is moving into maintenance mode.

## Building PicoGK Runtime

PicoGK Runtime is not distributed or built independently. It is included as a Git submodule in [PicoGK.Legacy](https://github.com/leap71/PicoGK.Legacy), which manages the native dependencies, cross-platform compilation, testing, and distribution.

If you want to build PicoGK 2.x from source, please use the PicoGK.Legacy superproject.

The runtime is based on the open-source [OpenVDB](https://github.com/AcademySoftwareFoundation/openvdb), [GLFW](https://github.com/glfw/glfw), and [Dear ImGui](https://github.com/ocornut/imgui) projects.

## PicoGK Repositories

- [**PicoGK**](https://github.com/leap71/PicoGK) — The existing C# geometry library and public API for PicoGK 2.x.
- [**PicoGKRuntime**](https://github.com/leap71/PicoGKRuntime) — The native C++ implementation of PicoGK 2.x (this repository).
- [**PicoGK.Legacy**](https://github.com/leap71/PicoGK.Legacy) — The unified build and distribution system for PicoGK 2.x.
- [**PicoGK.Core**](https://github.com/leap71/PicoGK.Core) — The next-generation geometry kernel, featuring a redesigned architecture and native runtime.

PicoGK 2.x will continue to receive maintenance and bug fixes, while new architectural development takes place in PicoGK.Core.

## License

PicoGK was originally developed by Lin Kayser and is maintained by [LEAP 71](https://leap71.com/).

PicoGK and PicoGK Runtime are released under the [Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0) (SPDX: Apache-2.0).