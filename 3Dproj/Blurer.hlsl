RWTexture2D<unorm float4> horBlurOutput;
Texture2D source : register(t1);
SamplerState testSampler;

//static const int WIDTH = 1920;
//static const int HEIGHT = 1080;

#define threadCount 32
#define maxRadius 4

[numthreads(threadCount, 8, 1)]
void main(uint3 groupThreadID : SV_GroupThreadID, uint3 dispatchThreadId : SV_DispatchThreadID)
{
    //int width, height;
    //int blurRadius = 4;
    //float4 CurrentBlurColor;
    //
    //source.GetDimensions(width, height);
    //int clamedDisX = min(dispatchThreadId.x, width - 1);
    //int clamedDisY = min(dispatchThreadId.y, height - 1);
    //uint bRadius = (uint) blurRadius;
    //if (groupThreadID.x < bRadius)
    //{
    //    int x = max(clamedDisX - blurRadius, 0);
    //    uint2 clampedPos = uint2(x, clamedDisY);
    //    CurrentBlurColor = source[clampedPos];
    //
    //}
    //else if (groupThreadID.x >= threadCount - bRadius) // 2 zakres
    //{
    //    int x = min(clamedDisX + blurRadius, width - 1);
    //    uint2 clampedPos = uint2(x, clamedDisY);
    //    CurrentBlurColor = source[clampedPos];
    //
    //}
    //
    //uint2 clampedPos = uint2(clamedDisX, clamedDisY);
    //CurrentBlurColor = source[clampedPos];
    //
    //GroupMemoryBarrierWithGroupSync();
    //float4 blurColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    //
    //[unroll(2*maxRadius)]
    //for (int i = -blurRadius; i <= blurRadius; i++)
    //{
    //    int k = groupThreadID.x + blurRadius + i;
    //    blurColor += CurrentBlurColor;
    //
    //}
    //horBlurOutput[uint2(dispatchThreadId.x, dispatchThreadId.y)] = blurColor * 1.0f / (blurRadius * 2 + 1);
    //horBlurOutput[dispatchThreadId.xy] = float4(dispatchThreadId.x / 1920.f, dispatchThreadId.y / 1080.f, 0, 1);
    
    
    /*
    int x = DTid.x;
    int y = DTid.y;
    
    int takeMinX = (x < 1) ? x : x - 1;
    int takeMinY = (y < 1) ? y : y - 1;
    int takeMaxX = (x >= WIDTH - 1) ? x : x + 1;
    int takeMaxY = (x >= HEIGHT - 1) ? x : x + 1;
    float3 color = TheTexture.Load(int3(DTid.xy, 0));
    
    for (int o = takeMinX; o < takeMaxX; o++)
    {
        for (int p = takeMinY; p < takeMaxY; p++)
        {
            
        }
    }
    */
    
    //float2 uv = float2(dispatchThreadId.x / 1920.f, dispatchThreadId.y / 1080.f);
    
    float4 color = source.Load(int3(dispatchThreadId.xy, 0));
    if (color.w < 0.01f)
    {
        horBlurOutput[dispatchThreadId.xy] = float4(1,0,0,1);
    }
    else
    {
        horBlurOutput[dispatchThreadId.xy] = float4(color.xyzw);
    }
    

}