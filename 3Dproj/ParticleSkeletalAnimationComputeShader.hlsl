cbuffer Time
{
    float dt;
    float time;
    float2 padding;
};

static const int maxNumberOfBones = 50;
//need to check padding and other
cbuffer OBBSkeleton : register(b1)
{
    matrix Transformations[maxNumberOfBones]; //max number of bones are 50 (NOT FINAL!)
    float3 WidthHeightDepth[maxNumberOfBones]; //WIDTH and DEPTH are the same for all bones right now
    int nrOfBones;
};

RWBuffer<float> particleData;

[numthreads(16, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    static const float drag = 0.1f;
    static const float force = 0.9f;
    //pos 3
    //color 4
    //velocity 3
    float3 currPos = float3(particleData[DTid.x * 10 + 0], particleData[DTid.x * 10 + 1], particleData[DTid.x * 10 + 2]);
    float3 currentVelocity = float3(particleData[DTid.x * 10 + 7], particleData[DTid.x * 10 + 8], particleData[DTid.x * 10 + 9]);

    if (abs(currPos.x) > 200)
    {
        currPos.x = 0;
        currentVelocity.x = 0;

    }
    
    currentVelocity *= (1 - drag);
    //DEBUG
    //currentVelocity += float3(sin(time) + , cos(time) * 1, 0);
    currentVelocity += float3(0, 0, 0);
    currPos += currentVelocity * dt;
   

    particleData[DTid.x * 10 + 0] = currPos.x;
    particleData[DTid.x * 10 + 1] = currPos.y; 
    particleData[DTid.x * 10 + 2] = currPos.z;
    
    particleData[DTid.x * 10 + 7] = currentVelocity.x;
    particleData[DTid.x * 10 + 8] = currentVelocity.y;
    particleData[DTid.x * 10 + 9] = currentVelocity.z;

}