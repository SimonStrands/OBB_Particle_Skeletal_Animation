#include "SkeletalData.hlsli"

cbuffer Time : register(b0)
{
    float dt;
    int3 random;
};

//need to check padding and other
cbuffer OBBSkeleton : register(b1)
{
    row_major matrix Transformations[MAXNUMBEROFBONES]; //max number of bones are 55 (NOT FINAL!)
    row_major matrix InverseTransform[MAXNUMBEROFBONES]; //max number of bones are 55 (NOT FINAL!)
    row_major matrix DeltaTransformations[MAXNUMBEROFBONES];
    int nrOfBones;
    int3 obbSkeletalPadding;
};

RWBuffer<float> particleData;

static const float3 downVec = float3(0, -1, 0);
[numthreads(32, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    static const float drag = 0.9f;
    //static const float drag = 1.0f;
    static const float force = 1.002f;
    //static const float force = 1.5000f;
    
    //LOAD DATA IN BETTER NAMES
    static const int BUFFERSIZE = 14;
    float3 currPos = float3(particleData[DTid.x * BUFFERSIZE + 0], particleData[DTid.x * BUFFERSIZE + 1], particleData[DTid.x * BUFFERSIZE + 2]);
    float4 currColor = float4(particleData[DTid.x * BUFFERSIZE + 3], particleData[DTid.x * BUFFERSIZE + 4], particleData[DTid.x * BUFFERSIZE + 5], particleData[DTid.x * BUFFERSIZE + 6]);
    float3 currentVelocity = float3(particleData[DTid.x * BUFFERSIZE + 7], particleData[DTid.x * BUFFERSIZE + 8], particleData[DTid.x * BUFFERSIZE + 9]);
    const float3 originalPosition = float3(particleData[DTid.x * BUFFERSIZE + 10], particleData[DTid.x * BUFFERSIZE + 11], particleData[DTid.x * BUFFERSIZE + 12]);
    int boneID = int(float(particleData[DTid.x * BUFFERSIZE + 13]));
    
    /////////////REAL CODE/////////////////////////
    float4 nPos;
    for (min12int i = 0; i < nrOfBones; i++)
    {
        nPos = mul(float4(currPos, 1.0f), InverseTransform[i]);
        if ((abs(nPos.x) <= 0.5) && (nPos.y <= 1 && nPos.y >= 0) && (abs(nPos.z) <= 0.5))
        {
            boneID = i;
        }
    }
    if (boneID == -1)
    {
        currColor.w = 0;
    }
    
    particleData[DTid.x * BUFFERSIZE + 3] = currColor.x;
    particleData[DTid.x * BUFFERSIZE + 4] = currColor.y;
    particleData[DTid.x * BUFFERSIZE + 5] = currColor.z;
    particleData[DTid.x * BUFFERSIZE + 6] = currColor.w;
    
    particleData[DTid.x * BUFFERSIZE + 13] = (float)boneID;
}