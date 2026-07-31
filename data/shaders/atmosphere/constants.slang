// constants.slang
//
// Physical constants for atmospheric scattering.
// Units:
//   - distances: megameters (Mm)
//   - scattering/extinction coefficients: per megameter
//

implementing atmosphere;

// Planet geometry ------------------------------------------------------------
public static const float GroundRadiusMM = 6.360;
public static const float AtmosphereRadiusMM = 6.460;

// Surface --------------------------------------------------------------------
// Average Earth-like ground reflectance.
// Used for ground bounce contribution.
public static const float3 GroundAlbedo = float3( 0.3 );

// Rayleigh scattering --------------------------------------------------------
//
// Nitrogen + oxygen molecular scattering.
// Values are wavelength dependent.
//
public static const float3 RayleighScatteringBase = float3( 5.802, 13.558, 33.100 );
public static const float RayleighAbsorptionBase = 0.0;

// Mie scattering -------------------------------------------------------------
//
// Aerosol scattering parameters.
//
public static const float MieScatteringBase = 3.996;
public static const float MieAbsorptionBase = 4.400;

// Ozone ----------------------------------------------------------------------
//
// Ozone absorption peak around 25km altitude.
// Coefficients are wavelength dependent.
//
public static const float3 OzoneAbsorptionBase = float3( 0.650, 1.881, 0.085 );

// Density profiles -----------------------------------------------------------

public static const float RayleighScaleHeightKM = 8.0;
public static const float MieScaleHeightKM = 1.2;
public static const float OzoneCenterHeightKM = 25.0;
public static const float OzoneWidthKM = 15.0;

// Phase function defaults ----------------------------------------------------
// Henyey-Greenstein asymmetry parameter.
public static const float DefaultMieG = 0.8;

public static const float2 TransmittanceTextureSize = float2( 256, 64 );
public static const float2 MultipleScatteringTextureSize = float2( 32, 32 );
public static const float2 SkyViewTextureSize = float2( 200, 200 );

