struct VertexShaderInput
{
    float3 position : POSITION;
    float4 color : COLOR;
    float3 velocity : VELOCITY;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 velocity : VELOCITY;
    row_major float4x4 modelView : MODELVIEW;
    row_major float4x4 projection : PR;
};

cbuffer CBuf
{
    row_major matrix transform; //model
    row_major matrix view;
    row_major matrix projection;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;

    output.position = float4(input.position, 1.f);
    output.color = input.color;
    output.velocity = input.velocity;
    output.modelView = mul(transform, view);
    output.projection = projection;

    return output;
}