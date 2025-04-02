#include "common.h"

// view constants
layout(std140, set = GLOBAL_SET, binding = VIEW_CONSTANTS_BINDING) uniform ViewConstants
{
    mat4 ProjectionMatrix;
    mat4 InverseProjectionMatrix;
    mat4 ViewMatrix;
    mat4 InverseViewMatrix;
    mat4 ViewProjectionMatrix;
    mat4 InverseViewProjectionMatrix;

    vec2 Viewport;
    float Near;
    float Far;

    vec3 CameraPosition;
    float Padding;

    vec3 CameraDirection;
    float Padding2;
} u_ViewConstants;

float LinearDepth(float depthSample)
{
    float linear = u_ViewConstants.Near * u_ViewConstants.Far / (u_ViewConstants.Far - depthSample * (u_ViewConstants.Far - u_ViewConstants.Near));
    return linear;
}