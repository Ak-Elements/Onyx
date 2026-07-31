import includes.samplers;

float4 linearSample( uint32_t textureIndex, float2 uv ) {
    const uint2 indices = uint2( textureIndex, LinearSamplerIndex );
    return sample( indices, uv );
}

float4 linearSampleClampEdge( uint32_t textureIndex, float2 uv ) {
    const uint2 indices = uint2( textureIndex, LinearSamplerIndex + SamplerClampEdgeUv );
    return sample( indices, uv );
}

float4 pointSample( uint32_t textureIndex, float2 uv ) {
    const uint2 indices = uint2( textureIndex, PointSamplerIndex );
    return sample( indices, uv );
}

float4 sample( uint2 indices, float2 uv ) {
    DescriptorHandle< Texture2D > bindlessTexture = DescriptorHandle< Texture2D >( uint2( indices.x, 0 ) );
    DescriptorHandle< SamplerState > bindlessSampler = DescriptorHandle< SamplerState >( uint2( 0, indices.y ) );
    return bindlessTexture.Sample( bindlessSampler, uv );
}
float4 sampleCombined( uint32_t textureIndex, float2 uv ) {
    DescriptorHandle< Sampler2D > combinedImage = DescriptorHandle< Sampler2D >( uint2( textureIndex, 0 ) );
    return combinedImage.Sample( uv );
}
