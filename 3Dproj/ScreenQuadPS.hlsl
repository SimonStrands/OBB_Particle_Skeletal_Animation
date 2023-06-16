Texture2D diffuseTex : register(t0);
SamplerState testSampler;

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 uv : UV;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    const float4 diffuseTexture = diffuseTex.Sample(testSampler, input.uv).xyzw;
    clip(diffuseTexture.w < 0.1f ? -1 : 1);
    return float4(0, 0, 1, diffuseTexture.w);
}