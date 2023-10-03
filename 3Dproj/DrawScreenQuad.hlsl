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

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = float4(input.position.xy, 0, 1.0f);
    output.uv = input.uv;
    return output;
}