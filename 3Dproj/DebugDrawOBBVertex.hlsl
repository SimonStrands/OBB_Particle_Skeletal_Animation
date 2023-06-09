#include "SkeletalData.hlsli"
cbuffer cbuf
{
    row_major matrix transform[MAXNUMBEROFBONES]; //model
    row_major matrix view;
    row_major matrix projection;
};

struct VertexShaderInput
{
    float3 position : POSITION;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 uv : UV;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4 fragpos : FRAG_POS;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    float4x4 MVP = mul(mul(transform[instanceID], view), projection);
    output.position = mul((float4(input.position, 1.0f)), MVP);
    output.uv = float2(0.5f, 0.5f);
    output.color = float4(1, 1, 1, 1);
    output.normal = output.tangent = output.bitangent = float3(1, 1, 1);
    output.fragpos = float4(input.position, 1.f);
    return output;
}