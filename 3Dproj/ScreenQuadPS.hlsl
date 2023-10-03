Texture2D<float4> diffuseTex : register(t0);
SamplerState testSampler
{
    AddressU = Clamp;
    AddressV = Clamp;
};

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 uv : UV;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    float2 texelSize = 1.0 / float2(1920, 1080);
    //float4 blurColor = float4(0.0, 0.0, 0.0, 0.0);
    //
    //// Blur in horizontal direction
    //int nrOfTimes = 0;
    //for (int i = -20; i <= 20; ++i)
    //{
    //    float2 offset = float2(i, 0) * texelSize;
    //    float2 uv = input.uv + offset;
    //    if (uv.x < 1 && uv.x >= 0 && uv.y < 1 && uv.y >= 0)
    //    {
    //        float4 addColor = diffuseTex.Sample(testSampler, saturate(uv));
    //        nrOfTimes++;
    //        blurColor += addColor;
    //    }
    //}
    //
    //// Normalize the blur color
    //blurColor /= (float)nrOfTimes;
    //clip(blurColor.w < 0.001f ? -1 : 1);
    //
    //return blurColor;
    
    float4 blurColor = float4(0.0, 0.0, 0.0, 0.0);
    //clip(diffuseTex.Sample(testSampler, input.uv).w > 0.5f ? 1 : -1);
    float nrOfTimes = 0;
    // Blur in horizontal direction
    for (int i = -50; i <= 50; ++i)
    {
        float2 offset = float2(i, 0) * texelSize;
        float4 addColor = diffuseTex.Sample(testSampler, saturate(input.uv + offset));
        if (addColor.w > 0.5f)
        {
            blurColor += addColor;
            nrOfTimes++;
        }
    }
    
    // Normalize the blur color
    blurColor /= nrOfTimes;
    
    return blurColor;
}