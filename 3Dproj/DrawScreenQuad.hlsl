struct VertexShaderInput
{
    float3 position : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT; //calc this in shader
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 uv : UV;
};

VertexShaderOutput main(VertexShaderInput input) : SV_POSITION
{
    VertexShaderOutput output;
    output.position = input.position;
    output.uv = input.uv;
    return output;
}