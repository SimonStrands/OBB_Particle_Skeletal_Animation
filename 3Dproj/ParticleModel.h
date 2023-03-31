#pragma once
#include <string>
#include "Vec.h"
#include "Graphics.h"
#include "CreateBuffer.h"
#include "ParticleModelLoader.h"

//TODO: standard color is empty/white

class ParticleModel{
public:
	ParticleModel(Graphics*& gfx, const std::string& filePath, vec3 position);
	~ParticleModel();
	void addAnimation(const std::string& filePath);
	void updateParticles(float dt, Graphics*& gfx);
	void draw(ID3D11DeviceContext*& immediateContext);
private:
	vec3 pos;
	vec3 rot;
	vec3 scale;
	float voxelScale;
private:
	void setShaders(ID3D11DeviceContext*& immediateContext);

	ID3D11InputLayout* inputLayout;
	UINT nrOfVertecies;

	//vertex geometry and pixel shader for drawing the particles
	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;
	ID3D11GeometryShader* GS;

	//vertex buffer with position, velocity and color
	ID3D11Buffer* vertexBuffer;

	//2 textures for the particle one diffuse and one normal map
	ID3D11ShaderResourceView* SRV;

	//compute shader for updating particle position
	ID3D11ComputeShader* cUpdate;
	ID3D11Buffer* computeShaderConstantBuffer;
	ID3D11UnorderedAccessView* billUAV;
};