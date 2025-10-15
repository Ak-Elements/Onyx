#include "includes/common.h"

// Bindless support //////////////////////////////////////////////////////
// Enable non uniform qualifier extension
#extension GL_EXT_nonuniform_qualifier : enable

layout(set = BINDLESS_SET, binding = BINDLESS_BINDING) uniform sampler2D BindlessTextures[];
// Alias textures to use the same binding point, as bindless texture is shared
// between all kind of textures: 1d, 2d, 3d.
layout(set = BINDLESS_SET, binding = BINDLESS_BINDING) uniform sampler3D BindlessTextures3D[];

layout(set = BINDLESS_SET, binding = BINDLESS_BINDING) uniform samplerCube BindlessTexturesCubemaps[];

layout(set = BINDLESS_SET, binding = BINDLESS_BINDING) uniform samplerCubeArray BindlessTexturesCubemapsArray[];

// Writeonly images do not need format in layout
layout(set = BINDLESS_SET, binding = BINDLESS_IMAGES) writeonly uniform image2D BindlessImages2D[];

layout(set = BINDLESS_SET, binding = BINDLESS_IMAGES) writeonly uniform uimage2D BindlessUImages2D[];
