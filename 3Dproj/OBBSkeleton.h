#pragma once
#include "Vec.h"
#include <vector>
#include "Graphics.h"
#include "point.h"
#include "ParticleModelLoader.h"

struct OBBSkeletonOBBBuffer : CB{
	struct{
		DirectX::XMMATRIX element[70];
	}transform;
	struct{
		DirectX::XMMATRIX element;
	}view;
	struct{
		DirectX::XMMATRIX element;
	}projection;
};

//static const float OBBWidth = 0.01f;
//static const float OBBDepth = 0.01f;
//static const float OBBWidth = 0.1f;
//static const float OBBDepth = 0.1f;


class OBBSkeletonDebug{
public:
	OBBSkeletonDebug(unsigned int nrOfBones, std::vector<DirectX::XMFLOAT3> &sizes, Graphics*& gfx);
	~OBBSkeletonDebug();
	void setTransformations(std::vector<DirectX::XMMATRIX>& transform);
	void setTransform(int id, const DirectX::XMMATRIX transform);
	std::vector<DirectX::XMMATRIX>& getTransforms();
	void updateObbPosition(Bone& rootjoint, const SkeletonConstantBuffer skeltonConstBuffer);
	void draw(Graphics*& gfx);
	ID3D11Buffer* getSkeletalTransformConstBuffer();

private:
	
	std::vector<DirectX::XMMATRIX> transform;//rotation position
	std::vector<DirectX::XMMATRIX> size;//the size of the OBB
	int nrOfBones;
	void update(Graphics*& gfx);
private:
	std::vector<point> verteciesPoints;
	std::vector<DWORD> indecies;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indeciesBuffer;
	ID3D11Buffer* constantBuffer;
	OBBSkeletonOBBBuffer constBufferConverter;
	
};