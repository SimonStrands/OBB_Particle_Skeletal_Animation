#include "Transforms.hlsli"

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 uv : UV;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4 fragpos : FRAG_POS;
};

Texture2D diffuseTex : register(t0);
Texture2D nMap : register(t1);
Texture2DArray<float4> shadowMapping : register(t5);
SamplerState testSampler;

float4 main(PixelShaderInput input) : SV_TARGET
{
    
    //check if the particle should disepear
    const float4 diffuseTexture = diffuseTex.Sample(testSampler, input.uv).xyzw;
    clip(diffuseTexture.w < 0.1f ? -1 : 1);
    clip(input.color.w < 0.1f ? -1 : 1);
    
    
    float4 lightning = float4(0, 0, 0, 1);
    static const float3 lightColor = float3(1, 1, 1);
    static const float3 ambient_light = float3(0.1f, 0.1f, 0.1f);
    
    float3 color = diffuseTexture.xyz * input.color.xyz;
    
    for (int i = 0; i < nrOfLight; i++)
    {
        float3 lightDir = normalize(lightPos[i].xyz - input.fragpos.xyz);
        const float4 shadowCamera = input.fragpos;
        const float4 shadowHomo = mul(shadowCamera, lightViewProj[i]);
        
        float4 shadowMapCoords = shadowHomo * float4(0.5, -0.5, 1.0f, 1.0f) + (float4(0.5f, 0.5f, 0.0f, 0.0f) * shadowHomo.w);
        shadowMapCoords.xyz = shadowMapCoords.xyz / shadowMapCoords.w;
        float4 SM = shadowMapping.SampleLevel(testSampler, float3(shadowMapCoords.x, shadowMapCoords.y, i), 0);
        
        //ambient
        float3 ambientColor = color.xyz * 0.4f;
        float bias = 0.00001f;
        if (SM.r > shadowMapCoords.z - bias &&
                shadowMapCoords.z <= 1.0f && //E
				shadowMapCoords.x < 1 && shadowMapCoords.x > 0 &&
				shadowMapCoords.y < 1 && shadowMapCoords.y > 0 &&
                dot(input.normal.xyz, lightDir.xyz) > -0.1
               )
        {
            float3 viewDir = normalize(cameraPos.xyz - input.fragpos.xyz);
            float3 halfWayDir = normalize(lightDir - viewDir);
                
			//////calc lightning//////
			//defuse
            float3 defuse_light;
            float ammount_diffuse = max(dot(input.normal.xyz, lightDir), 0.0f);
            defuse_light = ammount_diffuse * color.xyz * lightColor.xyz;
                
            //specular
            float3 reflectDir = reflect(-lightDir, input.normal.xyz);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2.0f);
            float3 specular = float3(0.5f,0.5f,0.5f) * spec;
                
            lightning.xyz += saturate(ambient_light + defuse_light) + specular;
        }
        else
        {
            lightning.xyz += ambientColor;

        }

    }
    return float4(lightning.xyz, input.color.w);
}