#pragma once

#include <onyx/onyx_types.h>
#include <onyx/geometry/common.h>

namespace Onyx::Graphics
{
    inline constexpr onyxU32 TRANSMITTANCE_TEXTURE_WIDTH = 256;
    inline constexpr onyxU32 TRANSMITTANCE_TEXTURE_HEIGHT = 64;

    inline constexpr onyxU32 IRRADIANCE_TEXTURE_WIDTH = 64;
    inline constexpr onyxU32 IRRADIANCE_TEXTURE_HEIGHT = 16;

    inline constexpr onyxU32 SCATTERING_TEXTURE_R_SIZE = 32;
    inline constexpr onyxU32 SCATTERING_TEXTURE_MU_SIZE = 128;
    inline constexpr onyxU32 SCATTERING_TEXTURE_MU_S_SIZE = 32;
    inline constexpr onyxU32 SCATTERING_TEXTURE_NU_SIZE = 8;

    inline constexpr onyxU32 SCATTERING_TEXTURE_WIDTH = SCATTERING_TEXTURE_NU_SIZE * SCATTERING_TEXTURE_MU_S_SIZE;
    inline constexpr onyxU32 SCATTERING_TEXTURE_HEIGHT = SCATTERING_TEXTURE_MU_SIZE;
    inline constexpr onyxU32 SCATTERING_TEXTURE_DEPTH = SCATTERING_TEXTURE_R_SIZE;

    struct DensityProfileLayer
    {
        onyxF32 Width;
        onyxF32 ExpTerm;
        onyxF32 ExpScale;
        onyxF32 LinearTerm;
        onyxF32 ConstantTerm;
        onyxF32 Padding;
    };

    struct DensityProfile
    {
        DensityProfileLayer Layers[2];
    };

    /*
    The atmosphere parameters are then defined by the following struct:
    */
    struct AtmosphereParameters
    {
        // The solar irradiance at the top of the atmosphere.
        Vector3f SolarIrradiance;
        // The sun's angular radius. Warning: the implementation uses approximations
        // that are valid only if this angle is smaller than 0.1 radians.
        onyxF32 SunAngularRadius;
        
        // The density profile of air molecules, i.e. a function from altitude to
        // dimensionless values between 0 (null density) and 1 (maximum density).
        DensityProfile RayleighDensity;

        // The scattering coefficient of air molecules at the altitude where their
        // density is maximum (usually the bottom of the atmosphere), as a function of
        // wavelength. The scattering coefficient at altitude h is equal to
        // 'rayleigh_scattering' times 'rayleigh_density' at this altitude.
        Vector3f RayleighScattering;

        // The cosine of the maximum Sun zenith angle for which atmospheric scattering
        // must be precomputed (for maximum precision, use the smallest Sun zenith
        // angle yielding negligible sky light radiance values. For instance, for the
        // Earth case, 102 degrees is a good choice - yielding mu_s_min = -0.2).
        onyxF32 MuSMin;

        // The density profile of aerosols, i.e. a function from altitude to
        // dimensionless values between 0 (null density) and 1 (maximum density).
        DensityProfile MieDensity;

        // The scattering coefficient of aerosols at the altitude where their density
        // is maximum (usually the bottom of the atmosphere), as a function of
        // wavelength. The scattering coefficient at altitude h is equal to
        // 'mie_scattering' times 'mie_density' at this altitude.
        Vector3f MieScattering;

        // The distance between the planet center and the top of the atmosphere.
        onyxF32 TopRadius;

        // The extinction coefficient of aerosols at the altitude where their density
        // is maximum (usually the bottom of the atmosphere), as a function of
        // wavelength. The extinction coefficient at altitude h is equal to
        // 'mie_extinction' times 'mie_density' at this altitude.
        Vector3f MieExtinction;

        // The asymetry parameter for the Cornette-Shanks phase function for the
        // aerosols.
        onyxF32 MiePhaseFunctionG;
        // The density profile of air molecules that absorb light (e.g. ozone), i.e.
        // a function from altitude to dimensionless values between 0 (null density)
        // and 1 (maximum density).
        DensityProfile AbsorptionDensity;
        // The extinction coefficient of molecules that absorb light (e.g. ozone) at
        // the altitude where their density is maximum, as a function of wavelength.
        // The extinction coefficient at altitude h is equal to
        // 'absorption_extinction' times 'absorption_density' at this altitude.
        Vector3f AbsorptionExtinction;

        // The distance between the planet center and the bottom of the atmosphere.
        onyxF32 BottomRadius;

        // The average albedo of the ground.
        Vector3f GroundAlbedo;
        float Padding;
    };
}