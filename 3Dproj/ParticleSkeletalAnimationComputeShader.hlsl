#include "SkeletalData.hlsli"

cbuffer Time : register(b0)
{
    float dt;
    int random;
    float2 padding;
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

//https://gist.github.com/mattatz/86fff4b32d198d0928d0fa4ff32cf6fa

static const float3 downVec = float3(0, -1, 0);
[numthreads(16, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{   
    
    static const float drag = 0.9f;
    //static const float force = 1.0001f;
    static const float force = 1.0011f;
    
    //LOAD DATA IN BETTER NAMES
    float3 currPos = float3(particleData[DTid.x * 10 + 0], particleData[DTid.x * 10 + 1], particleData[DTid.x * 10 + 2]);
    float4 currColor = float4(particleData[DTid.x * 10 + 3], particleData[DTid.x * 10 + 4], particleData[DTid.x * 10 + 5], particleData[DTid.x * 10 + 6]);
    float3 currentVelocity = float3(particleData[DTid.x * 10 + 7], particleData[DTid.x * 10 + 8], particleData[DTid.x * 10 + 9]);
    
 
    //currColor = float4(0, 0, 1, 1);
    


    if (currPos.y <= 0)
    {
        int ourRandomNumber = (random * (DTid.x + 1));
        int randomBone = ourRandomNumber % (nrOfBones - 1);
        
        float4x4 temp = Transformations[randomBone];
        
        float deltaRandom = (dt * 1000000000) % 7;
        
        //x and z offset needs to be between -0.5 and 0.5 while y needs to be between 0 and 1
        float offsetX = ((ourRandomNumber % 100) / 100.f) - 0.5f;
        float offsetY = (((ourRandomNumber * 3) % 100.f) / 100);
        float offsetZ = (((ourRandomNumber * deltaRandom) % 100.f) / 100) - 0.5f;
    
        float4 offset = float4(offsetX, offsetY, offsetZ, 1.0f);
        float4 rotatedOffset = mul(offset, temp);
        
        //randomize a offset position 
        currPos = rotatedOffset;
        currentVelocity = float3(0,0,0);


        if (randomBone == 5 || (randomBone >= 9 &&randomBone <= 24)||(randomBone>=28&&randomBone<=43))
        {
            currColor = float4(0.3f, 0.4f, 0.f,1.f);
        }
        else
        {
            currColor = float4(0.0f, 0.6f, 3.f, 1.f);
        }
    }
    
    ///////////////REAL CODE/////////////////////////
    float4 nPos;
    int nrOfBonesEffected = 0;
    float3 velocities[3];
    
    //CHECKING IF POINT IS INSIDE AN OBB
    for (min12int i = 0; i < nrOfBones; i++)
    {
        nPos = mul(float4(currPos, 1.0f), InverseTransform[i]);
        if ((abs(nPos.x) <= 0.5) && (nPos.y <= 1 && nPos.y >= 0) && (abs(nPos.z) <= 0.5))
        {
            //Get velocity of max 3 bones
            velocities[nrOfBonesEffected] = mul(nPos, DeltaTransformations[i]).xyz;
            nrOfBonesEffected++;
            if (nrOfBonesEffected > 2)
            {
                break;
            }
    
            //Change the color of the particle
            //currColor = float4(0, 1, 0, 1);
        }
    }
    
    
    
    if (nrOfBonesEffected > 0)
    { 
        currentVelocity = float3(0, 0, 0);
        for (int i = 0; i < nrOfBonesEffected; i++)
        {
            currentVelocity += velocities[i] * force;
        }
        currentVelocity *= (1 / nrOfBonesEffected);
    }
    else
    {
        currentVelocity *= (1 - (drag * dt));
    
        //currColor = float4(1, 0, 0, 1);
    
        currentVelocity += float3(0, -9.81, 0) * dt * dt;
        
    }
    
    
    currPos += currentVelocity;
    
    
    /////////////////////////////////////////////////

    
    //SETS DATA
    particleData[DTid.x * 10 + 0] = currPos.x;
    particleData[DTid.x * 10 + 1] = currPos.y;
    particleData[DTid.x * 10 + 2] = currPos.z;

    particleData[DTid.x * 10 + 3] = currColor.x;
    particleData[DTid.x * 10 + 4] = currColor.y;
    particleData[DTid.x * 10 + 5] = currColor.z;
    particleData[DTid.x * 10 + 6] = currColor.w;

    particleData[DTid.x * 10 + 7] = currentVelocity.x;
    particleData[DTid.x * 10 + 8] = currentVelocity.y;
    particleData[DTid.x * 10 + 9] = currentVelocity.z;
}