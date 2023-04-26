#pragma once
#include <string>
#include "Vec.h"
#include "Graphics.h"
#include "CreateBuffer.h"
#include "ParticleModelLoader.h"
#include "OBBSkeleton.h"
//#include <cmath>
#include <fstream>

//for skeletal animation and moving particles
struct ComputerShaderParticleModelConstBuffer : public CB{
	struct{
		DirectX::XMMATRIX element;
	}Transformations;
	struct{
		float element;
	}time;
	struct{
		float element[2];
	}padding;
};


class ParticleModel{
public:
	ParticleModel();
	ParticleModel(Graphics*& gfx, const std::string& filePath, vec3 position);//some weird way doesn't work in release if we create * new
	void init(Graphics*& gfx, const std::string& filePath, vec3 position);
	~ParticleModel();
	void addAnimation(const std::string& filePath);
	void updateParticles(float dt, Graphics*& gfx);
	void draw(Graphics*& gfx);
private:
	float voxelScale;
	DirectX::XMMATRIX positionMatris;
	OBBSkeletonDebug* OBBSkeleton;
private:
	void setShaders(ID3D11DeviceContext*& immediateContext);
	void updateShaders(Graphics*& gfx);

	ID3D11InputLayout* inputLayout;
	UINT nrOfVertecies;

	//vertex geometry and pixel shader for drawing the particles
	ID3D11VertexShader* VS;
	ID3D11PixelShader* PS;
	ID3D11GeometryShader* GS;

	//vertex buffer with position, velocity and color
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* Vg_pConstantBuffer;

	//For skeletal animation
	SkeletonConstantBuffer SkeletonConstBufferConverter;
	ID3D11Buffer* SkeletonConstBuffer;
	Animation animation;

	//2 textures for the particle one diffuse and one normal map
	ID3D11ShaderResourceView* diffuseTexture;
	ID3D11ShaderResourceView* normalMapTexture;

	//compute shader for updating particle position
	ID3D11ComputeShader* cUpdate;
	ID3D11UnorderedAccessView* billUAV;
	ID3D11Buffer* computeShaderConstantBuffer;
	//ComputerShaderParticleModelConstBuffer CSConstBuffer;

	void getPose(Bone& joint, const Animation& anim, float time, DirectX::XMMATRIX parentTransform = DirectX::XMMATRIX(
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	));
	std::pair<unsigned int, float> getTimeFraction(const std::vector<float>& times, float& dt);

	//should never be 0
	float time = 0.000001f;
	Bone rootJoint;
	bool hasAnimation;

	
};