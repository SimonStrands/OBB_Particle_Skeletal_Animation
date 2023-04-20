#include "SkeletalData.hlsli"
//cbuffer Time
//{
//    float dt;
//    float time;
//    float2 padding;
//};

//need to check padding and other
cbuffer OBBSkeleton : register(b1)
{
    row_major matrix Transformations[MAXNUMBEROFBONES]; //max number of bones are 70 (NOT FINAL!)
    row_major matrix view;
    row_major matrix projection;
    int nrOfBones;
};

RWBuffer<float> particleData;

//For more information at
//https://gist.github.com/mattatz/86fff4b32d198d0928d0fa4ff32cf6fa

[numthreads(16, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    
    static const float drag = 0.1f;
    static const float force = 0.9f;
    
    //pos 3
    //color 4
    //velocity 3
    
    //LOAD DATA IN BETTER NAMES
    float3 currPos = float3(particleData[DTid.x * 10 + 0], particleData[DTid.x * 10 + 1], particleData[DTid.x * 10 + 2]);
    float4 currColor = float4(particleData[DTid.x * 10 + 3], particleData[DTid.x * 10 + 4], particleData[DTid.x * 10 + 5], particleData[DTid.x * 10 + 6]);
    float3 currentVelocity = float3(particleData[DTid.x * 10 + 7], particleData[DTid.x * 10 + 8], particleData[DTid.x * 10 + 9]);


    ///////////////REAL CODE/////////////////////////
    currColor = float4(0, 0, 1, 1);
    float3 nPos;

    for (min12int i = 0; i < nrOfBones; i++)
    {
        //CHECK IF POINTS IS INSIDE A BONE OR NOT
        nPos = mul(float4(currPos, 1.0f), Transformations[i]).xyz;
        
        // Y will probably be change when the boxes starts at y=0 instead of in the middle of the box
        if ((abs(nPos.x) < 0.5) && (nPos.y < 1 && nPos.y > 0) && (abs(nPos.z) < 0.5))
        {
             //FOR DEBUG JUST CHANGE THE COLOR FOR NOW
             currColor = float4(0, 1, 0, 1);
             break;
        }
    }
    
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