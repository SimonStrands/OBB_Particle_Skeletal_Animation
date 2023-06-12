#include "SkeletalData.hlsli"

cbuffer Time : register(b0)
{
    float dt;
    int3 random;
};

//need to check padding and other
cbuffer OBBSkeleton : register(b1)
{
    matrix Transformations[MAXNUMBEROFBONES]; //max number of bones are 55 (NOT FINAL!)
    //row_major matrix Transformations[MAXNUMBEROFBONES]; //max number of bones are 55 (NOT FINAL!)
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
    int BufferBoneID = int(float(particleData[DTid.x * BUFFERSIZE + 13]));
    int boneID = BufferBoneID;
    
    while (boneID > 100)
    {
        boneID -= 100;
    }
    
    if (currPos.y <= -5)
    {
        float4x4 temp = Transformations[boneID];
    
        float4 offset = float4(originalPosition, 1.0f);
        
        //randomize a offset position 
        currPos = mul(offset, temp).xyz;
        currentVelocity = float3(0, 0, 0);
    
        particleData[DTid.x * BUFFERSIZE + 0] = currPos.x;
        particleData[DTid.x * BUFFERSIZE + 1] = currPos.y;
        particleData[DTid.x * BUFFERSIZE + 2] = currPos.z;
    }
    
    
    ///////////////REAL CODE/////////////////////////
    float4 nPos;
    int nrOfBonesEffected = 0;
    float3 velocities[3];
    
    nPos = mul(float4(currPos, 1.0f), InverseTransform[boneID]);
    if ((abs(nPos.x) <= 0.5) && (nPos.y <= 1 && nPos.y >= 0) && (abs(nPos.z) <= 0.5))
    {
        //Get velocity of max 3 bones
        velocities[nrOfBonesEffected] = mul(nPos, DeltaTransformations[boneID]).xyz;
        nrOfBonesEffected++;
        BufferBoneID = boneID += 100;
    }
    
    if (nrOfBonesEffected > 0)
    {
        currentVelocity = float3(0, 0, 0);
        for (int i = 0; i < nrOfBonesEffected; i++)
        {
            currentVelocity += velocities[i] * force * (1.f / nrOfBonesEffected);
        }
    }
    else
    {
        currentVelocity *= (1 - (drag * dt));
    
        while (BufferBoneID > 100)
        {
            BufferBoneID -= 100;
        }
    
        currentVelocity += float3(0, -9.81f, 0) * dt * dt * 10;
        
    }
    
    
    currPos += currentVelocity;
    
    
    /////////////////////////////////////////////////

    
    //SETS DATA
    particleData[DTid.x * BUFFERSIZE + 0] = currPos.x;
    particleData[DTid.x * BUFFERSIZE + 1] = currPos.y;
    particleData[DTid.x * BUFFERSIZE + 2] = currPos.z;

    particleData[DTid.x * BUFFERSIZE + 3] = currColor.x;
    particleData[DTid.x * BUFFERSIZE + 4] = currColor.y;
    particleData[DTid.x * BUFFERSIZE + 5] = currColor.z;
    particleData[DTid.x * BUFFERSIZE + 6] = currColor.w;

    particleData[DTid.x * BUFFERSIZE + 7] = currentVelocity.x;
    particleData[DTid.x * BUFFERSIZE + 8] = currentVelocity.y;
    particleData[DTid.x * BUFFERSIZE + 9] = currentVelocity.z;
    
    particleData[DTid.x * BUFFERSIZE + 13] = BufferBoneID;

}