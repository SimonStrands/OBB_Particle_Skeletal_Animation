struct VertexShaderInput
{
    float3 position : POSITION;
    float4 color : COLOR;
    float3 velocity : VELOCITY;
    int4 bondIDS : BONEID;
    float4 boneWeights : BoneWeight;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 velocity : VELOCITY;
    row_major float4x4 model : MODEL;
    row_major float4x4 view : VIEW;
    row_major float4x4 projection : PR;
};

cbuffer CBuf
{
    row_major matrix transform; //model
    row_major matrix view;
    row_major matrix projection;
};
static const int maxNumberOfBones = 50;
//need to check padding and other
cbuffer OBBSkeleton : register(b1)
{
    matrix Transformations[maxNumberOfBones]; //max number of bones are 50 (NOT FINAL!)
    int nrOfBones;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;

    //don't know if it should be all 0
    float4x4 boneTransform =
    {

        0.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 0.f
	};
    
    if (input.boneWeights.x > 0)
    {
        boneTransform += mul(Transformations[int(input.bondIDS.x)], input.boneWeights.x);
    }
    if (input.boneWeights.y > 0)
    {
        boneTransform += mul(Transformations[int(input.bondIDS.y)], input.boneWeights.y);
    }
    if (input.boneWeights.z > 0)
    {
        boneTransform += mul(Transformations[int(input.bondIDS.z)], input.boneWeights.z);
    }
    if (input.boneWeights.w > 0)
    {
        boneTransform += mul(Transformations[int(input.bondIDS.w)], input.boneWeights.w);
    }
    output.color = input.color;

    output.position = mul(float4(input.position, 1.0f), boneTransform);
    output.velocity = input.velocity;
    output.model = transform;
    output.view = view;
    output.projection = projection;

    return output;
}