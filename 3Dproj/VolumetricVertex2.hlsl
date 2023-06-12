struct VertexShaderInput
{
    float3 position : POSITION;
    float4 color : COLOR;
    float3 velocity : VELOCITY;
    float3 orginalPosition : ORIGINALPOSITION;
    int boneID : BONEID;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    row_major float4x4 model : MODEL;
    row_major float4x4 view : VIEW;
    row_major float4x4 projection : PR;
    //only for specific case
    bool Scale : Scale;
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
    
    output.Scale = false;
    if (input.boneID > 100)
    {
        output.Scale = true;
    }
    
    output.model = transform;
    output.view = view;
    output.projection = projection;

    return output;
}