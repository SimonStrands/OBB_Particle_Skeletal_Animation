struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 uv : UV;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 b = input.position;
    //we don't have alpha yeet
    return float4(input.color);
}