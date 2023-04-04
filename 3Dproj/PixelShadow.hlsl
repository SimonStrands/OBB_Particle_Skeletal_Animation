struct PixelShaderInput
{
   float4 position : SV_POSITION;
};

Texture2D SM : register(t4);

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 a = input.position;
	return float4(1,0,0,0.5f);
}

