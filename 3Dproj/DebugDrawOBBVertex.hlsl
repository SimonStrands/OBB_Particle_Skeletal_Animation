cbuffer cbuf
{
    row_major matrix transform[50]; //model
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
};

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    float4x4 MVP = mul(mul(transform[instanceID], view), projection);
    output.position = mul((float4(input.position, 1.0f)), MVP);
    return output;
}