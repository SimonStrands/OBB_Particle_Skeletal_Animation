struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 uv : UV;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4 fragpos : FRAG_POS;
};

Texture2D diffuseTex : register(t0);
Texture2D SM : register(t4);
SamplerState testSampler;

float4 main(PixelShaderInput input) : SV_TARGET
{
    const float4 diffuseTexture = diffuseTex.Sample(testSampler, input.uv).xyzw;
    clip(diffuseTexture.w < 0.1f ? -1 : 1);
    clip(input.color.w < 0.0001f ? -1 : 1);
	float4 a = input.position;
	return float4(1,0,0,1.0f);
}

