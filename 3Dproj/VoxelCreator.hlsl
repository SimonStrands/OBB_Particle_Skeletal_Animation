
struct GSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 velocity : VELOCITY;
    row_major float4x4 model : MODEL;
    row_major float4x4 view : VIEW;
    row_major float4x4 projection : PR;
};

struct GSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 velocity : VELOCITY;
    float4 fragpos : FRAG_POS;
};

[maxvertexcount(17)]
void main(
	point GSInput input[1],
	inout TriangleStream<GSOutput> output
)
{
    float4x4 modelView = mul(input[0].model, input[0].view);
    float4x4 MVP = mul(modelView, input[0].projection);
    float size = 0.02;

    float4 shadowCamera;
    float4 shadowHomo;

    GSOutput element;
    float4 v;

    element.color = input[0].color;
    element.velocity = input[0].velocity;
    
    v = input[0].position + float4(-1, 1, 1, 0.f) * size; //1
    element.position = mul(v, MVP);
    element.fragpos = mul(v, input[0].model);
    output.Append(element);
    
    v = input[0].position + float4(-1, -1, 1, 0.f) * size; //2
    element.position = mul(v, MVP);
    element.fragpos = mul(v, input[0].model);
    output.Append(element);

    v = input[0].position + float4(1, 1, 1, 0.f) * size; //3
    element.position = mul(v, MVP);
    element.fragpos = mul(v, input[0].model);
    output.Append(element);
    
    v = input[0].position + float4(1, -1, 1, 0.f) * size; //4
    element.position = mul(v, MVP);
    element.fragpos = mul(v, input[0].model);
    output.Append(element);
    
    v = input[0].position + float4(1, 1, -1, 0.f) * size; //5
    element.position = mul(v, MVP);
    element.fragpos = mul(v, input[0].model);
    output.Append(element);

    v = input[0].position + float4(1, -1, -1, 0.f) * size; //6
    element.position = mul(v, MVP);
    element.fragpos = mul(v, input[0].model);
    output.Append(element);
    
    v = input[0].position + float4(-1, 1, -1, 0.f) * size; //7
    element.position = mul(v, MVP);
    element.fragpos = mul(v, input[0].model);
    output.Append(element);

    v = input[0].position + float4(-1, -1, -1, 0.f) * size; //8
    element.position = mul(v, MVP);
    element.fragpos = mul(v, input[0].model);
    output.Append(element);
    
    v = input[0].position + float4(-1, 1, 1, 0.f) * size; //9
    element.position = mul(v, MVP);
    element.fragpos = mul(v, input[0].model);
    output.Append(element);
    
    v = input[0].position + float4(-1, -1, 1, 0.f) * size; //10
    element.position = mul(v, MVP);
    element.fragpos = mul(v, input[0].model);
    output.Append(element);

    v = input[0].position + float4(1, -1, 1, 0.f) * size; //11
    element.position = mul(v, MVP);
    element.fragpos = mul(v, input[0].model);
    output.Append(element);
    
    v = input[0].position + float4(-1, -1, -1, 0.f) * size; //12
    element.position = mul(v, MVP);
    element.fragpos = mul(v, input[0].model);
    output.Append(element);

    v = input[0].position + float4(1, -1, -1, 0.f) * size; //13
    element.position = mul(v, MVP);
    element.fragpos = mul(v, input[0].model);
    output.Append(element);
    
    v = input[0].position + float4(-1, 1, 1, 0.f) * size; //1/14
    element.position = mul(v, MVP);
    element.fragpos = mul(v, input[0].model);
    output.Append(element);
    
    v = input[0].position + float4(-1, 1, -1, 0.f) * size; //7/15
    element.position = mul(v, MVP);
    element.fragpos = mul(v, input[0].model);
    output.Append(element);
    
    v = input[0].position + float4(1, 1, 1, 0.f) * size; //3/17
    element.position = mul(v, MVP);
    element.fragpos = mul(v, input[0].model);
    output.Append(element);

    v = input[0].position + float4(1, 1, -1, 0.f) * size; //5/16
    element.position = mul(v, MVP);
    element.fragpos = mul(v, input[0].model);
    output.Append(element);
    
}