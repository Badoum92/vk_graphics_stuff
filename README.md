# Graphics stuff using Vulkan

Learning graphics programming with Vulkan

## Build

Tested on Windows 10 only
```
$ git clone --recursive
$ cd vk_graphics_stuff
$ mkdir build && cd build
$ cmake .. -G Ninja
$ ninja
```

## Todo

- [X] Basic Vulkan backend
- [X] glTF model loading
- [X] ImGUI support
- [X] Vox model loader
- [X] Compute shader ray tracing
- [ ] Lights and shadows
- [ ] PBR
- [ ] Ray-tracing pipeline (RTX)
- [ ] Some kind of ECS

## Progress

### 10/12/2021 - Simple voxel ray tracing
Implemented using the [Amanatides & Woo “A Fast Voxel Traversal Algorithm For Ray Tracing”](https://www.researchgate.net/publication/2611491_A_Fast_Voxel_Traversal_Algorithm_for_Ray_Tracing) algorithm.


Model found in this repo: https://github.com/ephtracy/voxel-model

![Screenshot](https://cdn.discordapp.com/attachments/892059047145119765/918872825203400775/voxels.png)
![Screenshot](https://cdn.discordapp.com/attachments/892059047145119765/918863915197292555/voxels.png)

### 03/10/2021 - ImGUI + Tone mapping
![Screenshot](https://cdn.discordapp.com/attachments/892059047145119765/894162362666450974/imgui_tonemap.png)


### 27/09/2021 - glTF models
![Screenshot](https://cdn.discordapp.com/attachments/892059047145119765/892059083270672494/sponza.png)
