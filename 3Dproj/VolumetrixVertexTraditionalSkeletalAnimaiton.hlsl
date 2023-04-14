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
static const int maxNumberOfBones = 70;
//need to check padding and other
cbuffer OBBSkeleton : register(b1)
{
    matrix Transformations[maxNumberOfBones]; //max number of bones are 70 (NOT FINAL!)
    int nrOfBones;
};

bool f4eqf4(float4x4 a, float4x4 b)
{
    bool theReturn = true;
    for (int x = 0; x < 4 && theReturn; x++)
    {
        for (int y = 0; y < 4 && theReturn; y++)
        {
            if (a[x][y] != b[x][y])
            {
                theReturn = false;
            }

        }
    }
    return theReturn;      
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
    
    if (input.bondIDS.x > -0.5f)
    {
        boneTransform += mul(Transformations[int(input.bondIDS.x)], input.boneWeights.x);
    }
    if (input.bondIDS.y > -0.5f)
    {
        boneTransform += mul(Transformations[int(input.bondIDS.y)], input.boneWeights.y);
    }
    if (input.bondIDS.z > -0.5f)
    {
        boneTransform += mul(Transformations[int(input.bondIDS.z)], input.boneWeights.z);
    }
    if (input.bondIDS.w > -0.5f)
    {
        boneTransform += mul(Transformations[int(input.bondIDS.w)], input.boneWeights.w);
    }
    
    //DEBUG
    static const float4x4 zeroVector =
    {
        0.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 0.f
    };
    static const float4x4 identity =
    {
        1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
    };
    if (f4eqf4(boneTransform, zeroVector))
    {
        boneTransform = identity;
    }
    
    output.color = input.color;
    
    output.position = mul(float4(input.position, 1.0f), boneTransform);
    output.velocity = input.velocity;
    output.model = transform;
    output.view = view;
    output.projection = projection;

    return output;
}