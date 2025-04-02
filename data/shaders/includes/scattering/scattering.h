const float PI = 3.14159265358;

// Units are in kilometers.
const float groundRadiusMM = 6.360;
const float atmosphereRadiusMM = 6.460;

const uint TRANSMITTANCE_TEXTURE_WIDTH = 256;
const uint TRANSMITTANCE_TEXTURE_HEIGHT = 64;
const vec2 TRANSMITTANCE_TEXTURE_SIZE = vec2(TRANSMITTANCE_TEXTURE_WIDTH, TRANSMITTANCE_TEXTURE_HEIGHT);

const uint MULTIPLE_SCATTERING_TEXTURE_WIDTH = 32;
const uint MULTIPLE_SCATTERING_TEXTURE_HEIGHT = 32;
const vec2 MULTPILE_SCATTERING_TEXTURE_SIZE = vec2(MULTIPLE_SCATTERING_TEXTURE_WIDTH, MULTIPLE_SCATTERING_TEXTURE_HEIGHT);

const uint SKY_VIEW_TEXTURE_WIDTH = 200;
const uint SKY_VIEW_TEXTURE_HEIGHT = 200;
const vec2 SKY_VIEW_TEXTURE_SIZE = vec2(SKY_VIEW_TEXTURE_WIDTH, SKY_VIEW_TEXTURE_HEIGHT);

const vec3 groundAlbedo = vec3(0.3);

// These are per megameter.
const vec3 rayleighScatteringBase = vec3(5.802, 13.558, 33.1);
const float rayleighAbsorptionBase = 0.0;

const float mieScatteringBase = 3.996;
const float mieAbsorptionBase = 4.4;

const vec3 ozoneAbsorptionBase = vec3(0.650, 1.881, .085);

float safeacos(const float x)
{
    return acos(clamp(x, -1.0, 1.0));
}

// From https://gamedev.stackexchange.com/questions/96459/fast-ray-sphere-collision-code.
float RayIntersectSphere(vec3 ro, vec3 rd, float rad) 
{
    float b = dot(ro, rd);
    float c = dot(ro, ro) - rad*rad;
    if (c > 0.0f && b > 0.0) return -1.0;
    float discr = b*b - c;
    if (discr < 0.0) return -1.0;
    // Special case: inside sphere, use far discriminant
    if (discr > b*b) return (-b + sqrt(discr));
    return -b - sqrt(discr);
}

float MiePhaseFunction(float cosTheta)
{
    const float g = 0.8;
    const float scale = 3.0 / (8.0 * PI);

    float num =  (1.0 - g * g) * (1.0 + cosTheta * cosTheta);
    float denom = (2.0 + g * g) * pow(((1.0 + g * g) - (2.0 * g * cosTheta)), 1.5);

    return scale * (num / denom);
}

float RayleighPhaseFunction(float cosTheta)
{
    const float k = 3.0 / (16.0 * PI);
    return k * (1.0 + cosTheta * cosTheta);
}

void GetScatteringValues(vec3 pos, 
                      out vec3 rayleighScattering, 
                      out float mieScattering,
                      out vec3 extinction)
{
    float altitudeKM = (length(pos)-groundRadiusMM)*1000.0;
    // Note: Paper gets these switched up.
    float rayleighDensity = exp(-altitudeKM/8.0);
    float mieDensity = exp(-altitudeKM/1.2);
    
    rayleighScattering = rayleighScatteringBase*rayleighDensity;
    float rayleighAbsorption = rayleighAbsorptionBase*rayleighDensity;
    
    mieScattering = mieScatteringBase*mieDensity;
    float mieAbsorption = mieAbsorptionBase*mieDensity;
    
    vec3 ozoneAbsorption = ozoneAbsorptionBase*max(0.0, 1.0 - abs(altitudeKM-25.0)/15.0);
    
    extinction = rayleighScattering + rayleighAbsorption + mieScattering + mieAbsorption + ozoneAbsorption;
}

vec3 GetTransmittance(in sampler2D transmittanceTexture, vec3 position, vec3 sunDirection)
{
    float height = length(position);
    vec3 up = position / height;
    float sunCosZenithAngle = dot(sunDirection, up);

    vec2 uv = vec2(TRANSMITTANCE_TEXTURE_WIDTH * clamp(0.5 + 0.5 * sunCosZenithAngle, 0.0, 1.0),
                  TRANSMITTANCE_TEXTURE_HEIGHT * max(0.0, min(1.0, (height - groundRadiusMM) / (atmosphereRadiusMM - groundRadiusMM))));
    uv /= TRANSMITTANCE_TEXTURE_SIZE;
    return texture(transmittanceTexture, uv).xyz;
}

vec3 GetMultipleScattering(in sampler2D multipleScatteringTexture, vec3 position, vec3 sunDirection)
{
    float height = length(position);
    vec3 up = position / height;
    float sunCosZenithAngle = dot(sunDirection, up);

    vec2 uv = vec2(MULTIPLE_SCATTERING_TEXTURE_WIDTH * clamp(0.5 + 0.5 * sunCosZenithAngle, 0.0, 1.0),
                  MULTIPLE_SCATTERING_TEXTURE_HEIGHT * max(0.0, min(1.0, (height - groundRadiusMM) / (atmosphereRadiusMM - groundRadiusMM))));
    uv /= MULTPILE_SCATTERING_TEXTURE_SIZE;
    return texture(multipleScatteringTexture, uv).xyz;
}

vec3 GetSkyLuminance(in sampler2D skyViewLutTexture, vec3 cameraPosition, vec3 cameraDirection, vec3 sunDirection)
{
    float height = length(cameraPosition);
    vec3 up = cameraPosition / height;
    
    float horizonAngle = safeacos(sqrt(height * height - groundRadiusMM * groundRadiusMM) / height);
    float altitudeAngle = horizonAngle - acos(dot(cameraDirection, up));
    float azimuthAngle;
    if (abs(altitudeAngle) > ((0.5 * PI) - 0.0001))
    {
        // looking up/down the Y axis
        azimuthAngle = 0.0;
    }
    else
    {
        vec3 right = cross(sunDirection, up);
        vec3 forward = cross(up, right);
        
        vec3 projectedDirection = normalize(cameraDirection - up * (dot(cameraDirection, up)));
        float sinTheta = dot(projectedDirection, right);
        float cosTheta = dot(projectedDirection, forward);
        azimuthAngle = atan(sinTheta, cosTheta) + PI;
    }

    float v = 0.5 + 0.5 * sign(altitudeAngle) * sqrt(abs(altitudeAngle) * 2.0/PI);
    vec2 uv = vec2(azimuthAngle / (2.0* PI), v);
    return texture(skyViewLutTexture, uv).xyz;
}
