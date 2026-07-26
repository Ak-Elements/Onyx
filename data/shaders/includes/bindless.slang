import samplers;

float4 linearSample( uint32_t textureIndex, float2 uv ) {
    DescriptorHandle< Texture2D > bindlessTexture = DescriptorHandle< Texture2D >( uint2( textureIndex, 0 ) );
    DescriptorHandle< SamplerState > bindlessSampler = DescriptorHandle< SamplerState >(
        uint2( 0, LinearSamplerIndex ) );
    return bindlessTexture.Sample( bindlessSampler, uv );
}

float4 linearSampleClampEdge( uint32_t textureIndex, float2 uv ) {
    DescriptorHandle< Texture2D > bindlessTexture = DescriptorHandle< Texture2D >( uint2( textureIndex, 0 ) );
    DescriptorHandle< SamplerState > bindlessSampler = DescriptorHandle< SamplerState >(
        uint2( 0, LinearSamplerIndex + SamplerClampEdgeUv ) );
    return bindlessTexture.Sample( bindlessSampler, uv );
}

float4 pointSample( uint32_t textureIndex, float2 uv ) {
    DescriptorHandle< Texture2D > bindlessTexture = DescriptorHandle< Texture2D >( uint2( textureIndex, 0 ) );
    DescriptorHandle< SamplerState > bindlessSampler = DescriptorHandle< SamplerState >(
        uint2( 0, PointSamplerIndex ) );
    return bindlessTexture.Sample( bindlessSampler, uv );
}

float4 SampleCombined( uint32_t textureIndex, float2 uv ) {
    DescriptorHandle< Sampler2D > combinedImage = DescriptorHandle< Sampler2D >( uint2( textureIndex, 0 ) );
    return combinedImage.Sample( uv );
}
