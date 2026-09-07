import includes.samplers;

float4 linearSample( uint32_t textureIndex, float2 uv ) {
    const uint2 indices = uint2( textureIndex, LinearSamplerIndex );
    return sample( indices, uv );
}

float4 linearSampleGrad( uint32_t textureIndex, float2 uv, float2 gradX, float2 gradY ) {
    const uint2 indices = uint2( textureIndex, LinearSamplerIndex );
    return sampleGrad( indices, uv, gradX, gradY );
}

float4 linearSample( uint32_t textureIndex, float2 uv, uint32_t lod ) {
    const uint2 indices = uint2( textureIndex, LinearSamplerIndex );
    return sample( indices, uv, lod );
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

float4 sampleGrad( uint2 indices, float2 uv, float2 gradX, float2 gradY ) {
    DescriptorHandle< Texture2D > bindlessTexture = DescriptorHandle< Texture2D >( uint2( indices.x, 0 ) );
    DescriptorHandle< SamplerState > bindlessSampler = DescriptorHandle< SamplerState >( uint2( 0, indices.y ) );
    return bindlessTexture.SampleGrad( bindlessSampler, uv, gradX, gradY );
}

float4 sample( uint2 indices, float2 uv, uint32_t lod ) {
    DescriptorHandle< Texture2D > bindlessTexture = DescriptorHandle< Texture2D >( uint2( indices.x, 0 ) );
    DescriptorHandle< SamplerState > bindlessSampler = DescriptorHandle< SamplerState >( uint2( 0, indices.y ) );
    return bindlessTexture.SampleLevel( bindlessSampler, uv, float( lod   ) );
}

float4 sampleCombined( uint32_t textureIndex, float2 uv ) {
    DescriptorHandle< Sampler2D > combinedImage = DescriptorHandle< Sampler2D >( uint2( textureIndex, 0 ) );
    return combinedImage.Sample( uv );
}
