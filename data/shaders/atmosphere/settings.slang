//
// settings.slang
//
// Runtime atmosphere configuration.
//

implementing atmosphere;

// Atmosphere configuration ---------------------------------------------------

public struct AtmosphereSettings {
    // Planet geometry (Mm)
    public float GroundRadiusMM;
    public float AtmosphereRadiusMM;

    // Surface response
    public float3 GroundAlbedo;

    // Rayleigh scattering
    public float3 RayleighScatteringBase;
    public float RayleighAbsorptionBase;
    public float RayleighScaleHeightKM;

    // Mie scattering
    public float MieScatteringBase;
    public float MieAbsorptionBase;
    public float MieScaleHeightKM;

    // Ozone absorption
    public float3 OzoneAbsorptionBase;
    public float OzoneCenterHeightKM;
    public float OzoneWidthKM;

    // Phase function parameters
    public float MieG;
};

// Default Earth-like atmosphere ----------------------------------------------

public static AtmosphereSettings getEarthAtmosphereSettings() {
    AtmosphereSettings settings;

    settings.GroundRadiusMM = GroundRadiusMM;
    settings.AtmosphereRadiusMM = AtmosphereRadiusMM;

    settings.GroundAlbedo = GroundAlbedo;

    settings.RayleighScatteringBase = RayleighScatteringBase;
    settings.RayleighAbsorptionBase = RayleighAbsorptionBase;
    settings.RayleighScaleHeightKM = RayleighScaleHeightKM;

    settings.MieScatteringBase = MieScatteringBase;
    settings.MieAbsorptionBase = MieAbsorptionBase;
    settings.MieScaleHeightKM = MieScaleHeightKM;

    settings.OzoneAbsorptionBase = OzoneAbsorptionBase;
    settings.OzoneCenterHeightKM = OzoneCenterHeightKM;
    settings.OzoneWidthKM = OzoneWidthKM;

    settings.MieG = DefaultMieG;

    return settings;
}
