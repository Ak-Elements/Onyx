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

vec3 CalculatePointLights(uint clusterIndex, vec3 V, vec3 N, vec3 worldPosition, vec3 albedo, float metalness, float roughness)
{
	vec3 Lo = vec3(0, 0, 0);

	uint pointLightCount = sbo_LightGrid.Cells[clusterIndex].PointLightCount;
	uint lightIndexOffset = sbo_LightGrid.Cells[clusterIndex].PointLightOffset;
	for (uint i = 0; i < pointLightCount; ++i)
	{
		uint lightIndex = sbo_GlobalLightIndices.Indices[lightIndexOffset + i].PointLightIndex;
		PointLight light = SceneLights.PointLights[lightIndex];
		
		vec3 lightToFragPos = light.Position - worldPosition;
		
		vec3 L = normalize(lightToFragPos);

		float dist = length(lightToFragPos);
		float attenuation = 1.0 / (dist * dist);

		vec3 radiance = light.Color * light.Intensity * attenuation;
		Lo += BRDF(L, V, N, radiance, albedo, metalness, roughness);
	}

	return Lo;
}

vec3 CalculateSpotLights(uint clusterIndex, vec3 V, vec3 N, vec3 worldPosition, vec3 albedo, float metalness, float roughness)
{
	vec3 Lo = vec3(0, 0, 0);

	uint spotLightCount = sbo_LightGrid.Cells[clusterIndex].SpotLightCount;
	uint lightIndexOffset = sbo_LightGrid.Cells[clusterIndex].SpotLightOffset;
	for (int i = 0; i < spotLightCount; ++i)
	{
		uint lightIndex = sbo_GlobalLightIndices.Indices[lightIndexOffset + i].SpotLightIndex;
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

vec3 CalculatePBRLighting(vec3 worldPosition, vec3 worldNormal, vec3 cameraWorldPosition, uint clusterIndex, PBRMaterial material)
{
	vec3 N = normalize(worldNormal);
	vec3 V = normalize(cameraWorldPosition - worldPosition);

	// Specular contribution
	vec3 Lo = CalculateDirectionalLights(V, N, material.Albedo, material.Metalness, material.Roughness);
	Lo += CalculatePointLights(clusterIndex, V, N, worldPosition, material.Albedo, material.Metalness, material.Roughness);
	Lo += CalculateSpotLights(clusterIndex, V, N, worldPosition, material.Albedo, material.Metalness, material.Roughness);

	vec3 color = material.Albedo + Lo;
	return color;
}

uint GetClusterIndex(vec4 fragCoord, uvec4 clusterSize, float scale, float bias)
{
	 //Locating which cluster you are a part of
    uint zTile     = uint(max(log2(LinearDepth(fragCoord.z)) * scale + bias, 0.0));
    uvec3 tiles    = uvec3( uvec2( fragCoord.xy / clusterSize.w ), zTile);
    uint clusterIndex = tiles.x +
                     clusterSize.x * tiles.y +
                     (clusterSize.x * clusterSize.y) * tiles.z;  
	return clusterIndex;
}