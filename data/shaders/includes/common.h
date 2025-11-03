#extension GL_ARB_shader_draw_parameters : enable
#extension GL_ARB_enhanced_layouts : enable
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_ARB_gpu_shader_int64 : require
#extension GL_EXT_shader_16bit_storage : require
#extension GL_EXT_shader_explicit_arithmetic_types : require
#extension GL_EXT_scalar_block_layout : enable

#define BINDLESS_SET 0
#define GLOBAL_SET 1
#define LIGHTING_SET 2
#define MATERIAL_SET 3

#define BINDLESS_BINDING 0
#define BINDLESS_IMAGES 1
#define VIEW_CONSTANTS_BINDING 2

#include "includes/debug/print.h"

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