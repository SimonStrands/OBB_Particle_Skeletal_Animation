

struct GSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    row_major float4x4 model : MODEL;
    row_major float4x4 view : VIEW;
    row_major float4x4 projection : PR;
	//only for specific case
    bool Scale : Scale;
};
struct GSOutput
{
	float4 position : SV_POSITION;
	float2 uv : UV;
    float4 color : COLOR;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float4 fragpos: FRAG_POS;
};

cbuffer CBuf
{
	float4 cameraPos;
};

[maxvertexcount(4)]
void main(
	point GSInput input[1],
	inout TriangleStream< GSOutput > output
)
{
    GSOutput element;
    float3 nCP = float3(input[0].position.xyz) + float3(0, -1, 0);
    //element.color = input[0].color * length(nCP) * 1.5;
    element.color.xyz = input[0].color.xyz * length(nCP) * 0.010f;
    element.color.w = input[0].color.w;
	
    float4x4 modelView = mul(input[0].model, input[0].view);
	float4x4 MVP = mul(modelView, input[0].projection);
	float size = 0.200f;
    if (input[0].Scale)
    {
        size = 0.75f;
    }
	//float size = 0.008f;
	float3 side = float3(modelView[0][0], modelView[1][0], modelView[2][0]);
	float3 up = float3(modelView[0][1], modelView[1][1], modelView[2][1]);
	float3 camToPos = float3(cameraPos.x, cameraPos.y, cameraPos.z)
		- float3(input[0].position.x, input[0].position.y, input[0].position.z);

	float4 shadowCamera;
	float4 shadowHomo;

	
	//element.normal = normalize(float3(float3(modelView[0][2], modelView[1][2], modelView[2][2])));
    element.normal = normalize(-camToPos);
	element.tangent = normalize(side);
	element.bitangent = normalize(up);
    
	float4 v;

	v = input[0].position - float4(side - up, 0.f) * size;
	element.position = mul(v, MVP);
	element.fragpos = mul(v, input[0].model);
	element.uv = float2(0, 0);
	output.Append(element);

	v = input[0].position + float4(side + up, 0.f) * size;
	element.position = mul(v, MVP);
	element.fragpos = mul(v, input[0].model);
	element.uv = float2(1, 0);
	output.Append(element);

	v = input[0].position - float4(side + up, 0.f) * size;
	element.position = mul(v, MVP);
	element.fragpos = mul(v, input[0].model);
	element.uv = float2(0, 1);
	output.Append(element);

	v = input[0].position + float4(side - up, 0.f) * size;
	element.position = mul(v, MVP);
	element.fragpos = mul(v, input[0].model);
	element.uv = float2(1, 1);
	output.Append(element);


}