
This repository contains my reference code for following [Elie Michel's WebGPU c++ guide](https://eliemichel.github.io/LearnWebGPU/index.html)

I wanted as minimal a setup as possible for learning purposes.
This unfortunately caused some stumbling / correcting.
(no Dawn or make system + the webgpu-native standard having changed since the tutorial was written forced a lot of small corrections but was educational)

Dependencies:
--------------------------------------------------
Libraries:
- GLFW
- webgpu

Maybe build both from source (I kept running into c/c++ runtime library issues using prebuilt binaries)

Note.
The glfw3webgpu.c wrapper to get a glfw surface wasn't working out of the box so I dug out the windows specific stuff.



