#pragma once
#include <string>
#include "Vec.h"
#include "Graphics.h"
#include "CreateBuffer.h"
#include "ParticleModelLoader.h"
#include "OBBSkeleton.h"


//for skeletal animation and moving particles
struct ComputerShaderParticleModelConstBuffer : CB{
	struct{
		float element;
	}dt;
	struct{
		float element;
	}time;
	struct{
		float element[2];
	}padding;
};


static const int maxNumberOfBones = 50;
struct SkeletonConstantBuffer : CB{
	struct{
		DirectX::XMMATRIX element[maxNumberOfBones]; //max number of bones are 50 (NOT FINAL!)
	}Transformations;
};


class ParticleModel{
public:
	ParticleModel(Graphics*& gfx, const std::string& filePath, vec3 position);
	~ParticleModel();
	void addAnimation(const std::string& filePath);
	void updateParticles(float dt, Graphics*& gfx);
	void draw(Graphics*& gfx);
private:
	float voxelScale;
	DirectX::XMMATRIX positionMatris;
	OBBSkeletonDebug* OBBSkeleton;
	
	//Animation animation;

	std::pair<int, float> GetTimeFraction(std::vector<float>& times, float& dt);
	void getPose(Joint& joint, const Animation& anim, float time, DirectX::XMMATRIX parentTransform = DirectX::XMMATRIX(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	));

	void GetPose2(Joint& skeleton, Animation& animation , float dt, DirectX::XMMATRIX parentTransform);

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
	//DirectX::XMMATRIX GlobalInverseTransform;

	//2 textures for the particle one diffuse and one normal map
	ID3D11ShaderResourceView* diffuseTexture;
	ID3D11ShaderResourceView* normalMapTexture;

	//compute shader for updating particle position
	ID3D11ComputeShader* cUpdate;
	ID3D11UnorderedAccessView* billUAV;
	ID3D11Buffer* computeShaderConstantBuffer;
	ComputerShaderParticleModelConstBuffer CSConstBuffer;


	float time = 0.000001f;
	Joint rootJoint;



	
};