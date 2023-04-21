#include "SkeletalData.hlsli"
cbuffer Time
{
    float dt;
    float2 padding;
};

//need to check padding and other
cbuffer OBBSkeleton : register(b1)
{
    row_major matrix Transformations[MAXNUMBEROFBONES]; //max number of bones are 70 (NOT FINAL!)
    row_major matrix DeltaTransformations[MAXNUMBEROFBONES];
    row_major matrix view;
    row_major matrix projection;
    int nrOfBones;
};

RWBuffer<float> particleData;

//https://gist.github.com/mattatz/86fff4b32d198d0928d0fa4ff32cf6fa
/*
float4x4 extract_rotation_matrix(float4x4 m)
{
    float sx = length(float3(m[0][0], m[0][1], m[0][2]));
    float sy = length(float3(m[1][0], m[1][1], m[1][2]));
    float sz = length(float3(m[2][0], m[2][1], m[2][2]));

    // if determine is negative, we need to invert one scale
    float det = determinant(m);
    if (det < 0)
    {
        sx = -sx;
    }

    float invSX = 1.0 / sx;
    float invSY = 1.0 / sy;
    float invSZ = 1.0 / sz;

    m[0][0] *= invSX;
    m[0][1] *= invSX;
    m[0][2] *= invSX;
    m[0][3] = 0;

    m[1][0] *= invSY;
    m[1][1] *= invSY;
    m[1][2] *= invSY;
    m[1][3] = 0;

    m[2][0] *= invSZ;
    m[2][1] *= invSZ;
    m[2][2] *= invSZ;
    m[2][3] = 0;

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;

    return m;
}
*/
#define RANDOM_IA 16807
#define RANDOM_IM 2147483647
#define RANDOM_AM (1.0f/float(RANDOM_IM))
#define RANDOM_IQ 127773u
#define RANDOM_IR 2836
#define RANDOM_MASK 123459876

struct NumberGenerator {
    int seed; // Used to generate values.

    // Returns the current random float.
    float GetCurrentFloat() {
        Cycle();
        return RANDOM_AM * seed;
    }

    // Returns the current random int.
    int GetCurrentInt() {
        Cycle();
        return seed;
    }

    // Generates the next number in the sequence.
    void Cycle() {
        seed ^= RANDOM_MASK;
        int k = seed / RANDOM_IQ;
        seed = RANDOM_IA * (seed - k * RANDOM_IQ) - RANDOM_IR * k;

        if (seed < 0)
            seed += RANDOM_IM;

        seed ^= RANDOM_MASK;
    }

    // Cycles the generator based on the input count. Useful for generating a thread unique seed.
    // PERFORMANCE - O(N)
    void Cycle(const uint _count) {
        for (uint i = 0; i < _count; ++i)
            Cycle();
    }

    // Returns a random float within the input range.
    float GetRandomFloat(const float low, const float high) {
        float v = GetCurrentFloat();
        return low * (1.0f - v) + high * v;
    }

    // Sets the seed
    void SetSeed(const uint value) {
        seed = int(value);
        Cycle();
    }
};
const float3 downVec = float3(0, -1, 0);
[numthreads(16, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    NumberGenerator test;
    test.SetSeed(34);
    //test.seed = 6;

    //float new RandomNumber = randmobubmber * Tid.x
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
    //currColor = float4(0, 0, 1, 1);
    float4 nPos;

    currPos = float3(currPos + float3(0, -0.001, 0));
    currColor = float4(0, 0.3, 0.7, 1);
    if (currPos.y <= 0)
    {
        
        float a = test.GetRandomFloat(0,nrOfBones);
        //currPos = 
        float x = Transformations[a][0][3];
        float y = Transformations[a][1][3];
        float z = Transformations[a][2][3];
        currPos = float3(x, y, z);
        //currPos.y = 6;
    }


    for (min12int i = 0; i < nrOfBones; i++)
    {
        //CHECK IF POINTS IS INSIDE A BONE OR NOT
        nPos = mul(float4(currPos, 1.0f), Transformations[i]);


        // Y will probably be change when the boxes starts at y=0 instead of in the middle of the box
        if ((abs(nPos.x) <= 0.5) && (nPos.y <= 1 && nPos.y >= 0) && (abs(nPos.z) <= 0.5))
        {
            //FOR DEBUG JUST CHANGE THE COLOR FOR NOW

            currPos = float3(currPos + mul(nPos, DeltaTransformations[i]).xyz);
            currColor = float4(0, 1, 0, 1);
            test.seed = nPos.z;
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