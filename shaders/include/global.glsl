#ifndef GLOBAL_H
#define GLOBAL_H

#version 460

#extension GL_EXT_buffer_reference : require
#extension GL_EXT_nonuniform_qualifier : require

layout(set = 0, binding = 0) uniform sampler2D tex2D[];
layout(set = 0, binding = 0) uniform sampler3D tex3D[];
layout(set = 1, binding = 0, rgba8) uniform image2D image2D_rgba8[];
layout(set = 1, binding = 0, r8ui) uniform uimage3D image3D_r8[];

#define BUFFER(NAME)        layout(std430, buffer_reference, buffer_reference_align = 8) readonly buffer NAME
#define PUSH_CONSTANT(NAME) layout(std430, push_constant) uniform NAME
#define INPUT(N, NAME)      layout(location = N) in NAME
#define OUTPUT(N, NAME)     layout(location = N) out NAME

#define VERTEX_INDEX        gl_VertexIndex
#define GOBAL_INVOCATION_ID gl_GlobalInvocationID

#endif
