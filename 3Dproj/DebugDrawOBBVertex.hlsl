#include "SkeletalData.hlsli"
cbuffer cbuf
{
    row_major matrix transform[MAXNUMBEROFBONES]; //model
    row_major matrix DeltaTransformations[MAXNUMBEROFBONES];
    row_major matrix view;
    row_major matrix projection;
    int nrOfBones;
};

struct VertexShaderInput
{
    float3 position : POSITION;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    float4x4 MVP = mul(mul(transform[instanceID], view), projection);
    output.position = mul((float4(input.position, 1.0f)), MVP);
    return output;
}