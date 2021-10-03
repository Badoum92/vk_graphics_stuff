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
- [ ] Lights and shadows
- [ ] PBR
- [ ] Ray-tracing pipeline (RTX)
- [ ] Some kind of ECS

## Advancement screenshots

### 27/09/2021 - glTF models
![Screenshot](https://cdn.discordapp.com/attachments/892059047145119765/892059083270672494/sponza.png)

### 03/10/2021 - ImGUI + Tone mapping
![Screenshot](https://cdn.discordapp.com/attachments/892059047145119765/894162362666450974/imgui_tonemap.png)
