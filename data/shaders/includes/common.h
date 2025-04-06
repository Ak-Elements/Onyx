#extension GL_ARB_shader_draw_parameters : enable
#extension GL_ARB_enhanced_layouts : enable

#define BINDLESS_SET 0
#define GLOBAL_SET 1
#define LIGHTING_SET 2
#define MATERIAL_SET 3

#define BINDLESS_BINDING 0
#define BINDLESS_IMAGES 1
#define VIEW_CONSTANTS_BINDING 2

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

vec4 FromLinearToSRGB(vec4 linearRGB)
{
  bvec4 cutoff = lessThan(linearRGB, vec4(0.0031308));
  vec4 higher = vec4(1.055)*pow(linearRGB, vec4(1.0/2.4)) - vec4(0.055);
  vec4 lower = linearRGB * vec4(12.92);

  return mix(higher, lower, cutoff);
}

vec4 ToLinearFromSRGB(vec4 sRGB)
{
  bvec4 cutoff = lessThan(sRGB, vec4(0.04045));
  vec4 higher = pow((sRGB + vec4(0.055))/vec4(1.055), vec4(2.4));
  vec4 lower = sRGB/vec4(12.92);
  
  return mix(higher, lower, cutoff);
}