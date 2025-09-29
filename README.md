
This repository contains my reference code for following Elie Michel's WebGPU c++ guide.

I wanted as minimal setup as possible for learning purposed/ sanity and unfortunately this forced a lot of stumbling and refactoring.
(I chose not to use Dawn & the webgpu-native standard has changed a little)


Please note this will only work on windows
==================================================
Dependencies:
--------------------------------------------------
### External Libraries:
- GLFW
- webgpu

I built both from source as I kept running into C/C++ runtime library issues using prebuilt binaries

The rest should be available, as an example using MSCV:
cl /MT /EHsc /std:c++20 ../src/unity.cpp /I"..\include" /link /LIBPATH:"..\lib" Shell32.lib runtimeobject.lib oleaut32.lib opengl32.lib Gdi32.lib d3dcompiler.lib ole32.lib User32.lib ws2_32.lib userenv.lib ntdll.lib propsys.lib wgpu_native.lib glfw3.lib


