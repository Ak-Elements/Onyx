#include "lightdata.h"

const vec3 dielectric = vec3(0.04);
const float PI = 3.14159265359;

struct PBRMaterial
{
	vec3 Albedo;
	float Metalness;

	vec3 Normal;
	float Roughness;

	vec3 Emissive;
};

// Normal Distribution function --------------------------------------
float D_GGX(float dotNH, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
	return alpha2/((PI * denom*denom) + 0.0001); 
}

// Geometric Shadowing function --------------------------------------
float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	float GL = dotNL / (dotNL * (1.0 - k) + k);
	float GV = dotNV / (dotNV * (1.0 - k) + k);
	return GL * GV;
}

// Fresnel function ----------------------------------------------------
vec3 F_Schlick(vec3 albedo, float cosTheta, float metallic)
{
	vec3 F0 = mix(vec3(0.04), albedo, metallic); // * material.specular

	float r = 2 * (1 - 0.05);
	float t = min(max(2 - r, 0), 1);
	vec3 F = F0 + t * (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0 * r); 
	return F;    
}

// Specular BRDF composition --------------------------------------------
vec3 BRDF(vec3 L, vec3 V, vec3 N, vec3 radiance, vec3 albedo, float metallic, float roughness)
{
	// Precalculate vectors and dot products	
	vec3 H = normalize (V + L);
	float dotNV = clamp(dot(N, V), 0.0, 1.0) + 0.0001;
	float dotNL = clamp(dot(N, L), 0.0, 1.0) + 0.0001;
	float dotNH = clamp(dot(N, H), 0.0, 1.0) + 0.0001;

	vec3 color = vec3(0.0);

	if (dotNL > 0.0)
	{
		float rroughness = max(0.05, roughness);
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, roughness); 
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, rroughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		vec3 F = F_Schlick(albedo, dotNV, metallic);

		vec3 spec = D * F * G / (4.0 * dotNL * dotNV);

		color += spec * dotNL * radiance;
	}

	return color;
}

vec3 CalculateDirectionalLights(vec3 V, vec3 N, vec3 albedo, float metalness, float roughness)
{
	vec3 Lo = vec3(0, 0, 0);
	for (int i = 0; i < SceneLights.DirectionalLightCount; ++i)
	{
		DirectionalLight light = SceneLights.DirectionalLights[i];
		vec3 L = normalize(-light.Direction);

		vec3 radiance = light.Color * light.Intensity;
		Lo += BRDF(L, V, N, radiance, albedo, metalness, roughness);
	}

	return Lo;
}

vec3 CalculatePointLights(uint clusterIndex, vec3 V, vec3 N, vec3 worldPosition, vec3 albedo, float metalness, float roughness, uint debugFlag)
{
	vec3 Lo = vec3(0, 0, 0);

	uint pointLightCount = debugFlag == 2 ? sbo_LightGrid.Cells[clusterIndex].PointLightCount : SceneLights.PointLightCount; 
	uint lightIndexOffset = sbo_LightGrid.Cells[clusterIndex].PointLightOffset;
	for (uint i = 0; i < pointLightCount; ++i)
	{
		uint lightIndex = debugFlag == 2 ? sbo_GlobalLightIndices.Indices[lightIndexOffset + i].PointLightIndex : i;
		PointLight light = SceneLights.PointLights[lightIndex];
		
		vec3 lightToFragPos = light.Position - worldPosition;
		
		vec3 L = normalize(lightToFragPos);

		float distance = length(lightToFragPos);

        float attenuation = clamp(1.0 - (distance * distance) / (light.Radius * light.Radius), 0.0, 1.0);
		attenuation *= mix(attenuation, 1.0, 0.5);

		vec3 radiance = light.Color * light.Intensity * attenuation;
		Lo += BRDF(L, V, N, radiance, albedo, metalness, roughness);
	}

	return Lo;
}

vec3 CalculateSpotLights(uint clusterIndex, vec3 V, vec3 N, vec3 worldPosition, vec3 albedo, float metalness, float roughness, uint debugFlag)
{
	vec3 Lo = vec3(0, 0, 0);

	uint spotLightCount = debugFlag == 2 ? sbo_LightGrid.Cells[clusterIndex].SpotLightCount : SceneLights.SpotLightCount;
	uint lightIndexOffset = sbo_LightGrid.Cells[clusterIndex].SpotLightOffset;
	for (int i = 0; i < spotLightCount; ++i)
	{
		uint lightIndex = debugFlag == 2 ? sbo_GlobalLightIndices.Indices[lightIndexOffset + i].SpotLightIndex : i;
		SpotLight light = SceneLights.SpotLights[lightIndex];
		
		vec3 lightToFragPos = light.Position - worldPosition;
		float distance = length(lightToFragPos);
		
		vec3 L = normalize(lightToFragPos);
		
		float cutoff = cos(radians(light.Angle * 0.5));
		float scos = max(dot(L, light.Direction), cutoff);
		float rim = (1.0 - scos) / (1.0 - cutoff);

		float attenuation = clamp(1.0 - (distance * distance) / (light.Range * light.Range), 0.0, 1.0);
		attenuation = mix(attenuation, 1.0, light.Falloff);
		attenuation *= 1.0 - pow(max(rim, 0.001), light.AngleAttenuation);

		vec3 radiance = light.Color * light.Intensity * attenuation;
		Lo += BRDF(L, V, N, radiance, albedo, metalness, roughness);
	}

	return Lo;
}

vec3 ColorHeatMap(float heat)
{
	if (heat < 0.0)
		return vec3(0.0);
	else if (heat > 1.0)
		return vec3(1.0);

	const vec4 kRedVec4   = vec4(0.13572138, 4.61539260, -42.66032258, 132.13108234);
    const vec4 kGreenVec4 = vec4(0.09140261, 2.19418839, 4.84296658, -14.18503333);
    const vec4 kBlueVec4  = vec4(0.10667330, 12.64194608, -60.58204836, 110.36276771);
    const vec2 kRedVec2   = vec2(-152.94239396, 59.28637943);
    const vec2 kGreenVec2 = vec2(4.27729857, 2.82956604);
    const vec2 kBlueVec2  = vec2(-89.90310912, 27.34824973);
	
	heat = clamp(heat, 0.0, 1.0);
	vec4 v4 = vec4(1.0, heat, heat * heat, heat * heat * heat);
	vec2 v2 = v4.zw * v4.z;

	return vec3(
		dot(v4, kRedVec4) + dot(v2, kRedVec2),
		dot(v4, kGreenVec4) + dot(v2, kGreenVec2),
		dot(v4, kBlueVec4) + dot(v2, kBlueVec2)
	);
}

vec3 debug_colors[8] = vec3[](
   vec3(0, 0, 0),    vec3( 0,  0,  1), vec3( 0, 1, 0),  vec3(0, 1,  1),
   vec3(1,  0,  0),  vec3( 1,  0,  1), vec3( 1, 1, 0),  vec3(1, 1, 1)
);

vec3 CalculatePBRLighting(vec3 worldPosition, vec3 worldNormal, vec3 cameraWorldPosition, vec4 fragCoord, float scale, float bias, uint clusterIndex, PBRMaterial material, uint debugFlag)
{
    uint pointLightCount = sbo_LightGrid.Cells[clusterIndex].PointLightCount;
    uint spotLightCount = sbo_LightGrid.Cells[clusterIndex].SpotLightCount;

    if (debugFlag == 1)
    {	
        // light count per cluster
        //int totalLightCount = int(pointLightCount + spotLightCount);
        //if (totalLightCount == 0)
        //	--totalLightCount;
        //else if (totalLightCount == 2 * MAX_LIGHTS)
        //	totalLightCount++;

        //vec3 lightCountColor = ColorHeatMap(float(totalLightCount) / (2 * MAX_LIGHTS));
        //return lightCountColor;

        uint zTile     = uint(max(log2(LinearDepth(fragCoord.z)) * scale + bias, 0.0));
        // cluster view
        uint zIndex = uint(mod(zTile, 8.0));
        return debug_colors[zIndex];
    }

    vec3 N = normalize(worldNormal);
    vec3 V = normalize(cameraWorldPosition - worldPosition);

    // Specular contribution
    vec3 Lo = CalculateDirectionalLights(V, N, material.Albedo, material.Metalness, material.Roughness);
    Lo += CalculatePointLights(clusterIndex, V, N, worldPosition, material.Albedo, material.Metalness, material.Roughness, debugFlag);
    Lo += CalculateSpotLights(clusterIndex, V, N, worldPosition, material.Albedo, material.Metalness, material.Roughness, debugFlag);

    vec3 color = material.Albedo + Lo;
    return color;
}

uint GetClusterIndex(vec4 fragCoord, uvec3 clusterGridSize, uvec2 clusterSize, float scale, float bias)
{
    float linearDepth = LinearDepth(fragCoord.z);
    uint zTile     = uint(max(log2(linearDepth) * scale + bias, 0.0));
    uvec3 clusterIndex3D    = uvec3( uvec2( fragCoord.x / clusterSize.x, fragCoord.y / clusterSize.y ), zTile);
    uint clusterIndex = clusterIndex3D.x +
                     clusterGridSize.x * clusterIndex3D.y +
                     (clusterGridSize.x * clusterGridSize.y) * clusterIndex3D.z;  
	return clusterIndex;
}