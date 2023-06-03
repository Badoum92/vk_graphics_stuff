## Build

Tested on Windows 10 only
```
$ git clone git@github.com:Badoum92/vk_graphics_stuff.git
$ cd vk_graphics_stuff
$ mkdir build && cd build
$ cmake .. -G Ninja
$ ninja
```

## Progress

### Metallic and transparent materials
![Screenshot](https://cdn.discordapp.com/attachments/892059047145119765/922567505648701510/voxels.png)

### Simple voxel ray tracing
Implemented using the [Amanatides & Woo “A Fast Voxel Traversal Algorithm For Ray Tracing”](https://www.researchgate.net/publication/2611491_A_Fast_Voxel_Traversal_Algorithm_for_Ray_Tracing) algorithm.


Model found in this repo: https://github.com/ephtracy/voxel-model

![Screenshot](https://cdn.discordapp.com/attachments/892059047145119765/918872825203400775/voxels.png)
![Screenshot](https://cdn.discordapp.com/attachments/892059047145119765/918863915197292555/voxels.png)

### glTF models + ImGui
![Screenshot](https://cdn.discordapp.com/attachments/892059047145119765/894162362666450974/imgui_tonemap.png)
