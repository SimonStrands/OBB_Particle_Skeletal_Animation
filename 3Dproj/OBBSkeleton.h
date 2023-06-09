#pragma once
#include "Vec.h"
#include <vector>
#include "Graphics.h"
#include "point.h"
#include "ParticleModelLoader.h"

static const int MAXNUMBEROFBONES = 55;

struct OBBSkeletonOBBBufferTime : public CB {
	struct{
		float element;
	}dt;
	struct{
		int element[3];
	}random;
};

struct OBBSkeletonOBBBuffer : public CB{
	struct{
		DirectX::XMMATRIX element[MAXNUMBEROFBONES];
	}transform;
	struct{
		DirectX::XMMATRIX element[MAXNUMBEROFBONES];
	}InverseTransform;
	struct {
		DirectX::XMMATRIX element[MAXNUMBEROFBONES];
	}deltaTransform;
	struct {
		int element;
	}nrOfBones;
	struct{
		int padding[3];
	}padding;
	OBBSkeletonOBBBuffer operator-(const OBBSkeletonOBBBuffer& other)const
	{
		OBBSkeletonOBBBuffer temp;
		for (int i = 0; i < this->nrOfBones.element; i++)
			temp.transform.element[i] = this->transform.element[i] - other.transform.element[i];
		return temp;
	}
};

struct OBBSkeletonOBBBufferDebugDraw : public CB{
	struct{
		DirectX::XMMATRIX element[MAXNUMBEROFBONES];
	}transform;
	struct{
		DirectX::XMMATRIX element;
	}view;
	struct{
		DirectX::XMMATRIX element;
	}projection;
};


class OBBSkeletonDebug{
public:
	OBBSkeletonDebug();
	OBBSkeletonDebug(unsigned int nrOfBones, std::vector<DirectX::XMFLOAT3> &sizes, Graphics*& gfx);//some reason doesn't work in release mode
	~OBBSkeletonDebug();
	void init(unsigned int nrOfBones, std::vector<DirectX::XMFLOAT3> &sizes, Graphics*& gfx);
	void setTransformations(std::vector<DirectX::XMMATRIX>& transform);
	void setTransform(int id, const DirectX::XMMATRIX transform);
	std::vector<DirectX::XMMATRIX>& getTransforms();
	void updateObbPosition(Bone& rootjoint, const SkeletonConstantBuffer skeltonConstBuffer);
	void update(Graphics*& gfx, float dt);
	void draw(Graphics*& gfx);
	ID3D11Buffer*& getSkeletalTransformConstBuffer();
	ID3D11Buffer*& getSkeletalTimeConstBuffer();
	std::vector<DirectX::XMFLOAT3>& getSizes();

private:
	
	std::vector<DirectX::XMMATRIX> transform;//rotation position
	std::vector<DirectX::XMFLOAT3> sizes;
	std::vector<DirectX::XMMATRIX> size;//the size of the OBB
	void inverseTransforms();
	void inverseDeltaTransforms();
	
private:
	std::vector<point> verteciesPoints;
	std::vector<DWORD> indecies;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indeciesBuffer;

	ID3D11Buffer* constantBuffer;
	ID3D11Buffer* constantBufferTime;
	ID3D11Buffer* constantBufferDebugDraw;

	OBBSkeletonOBBBufferDebugDraw constBufferConverterDebugDraw;
	OBBSkeletonOBBBufferTime constBufferConverterTime;
	OBBSkeletonOBBBuffer constBufferConverter;
	OBBSkeletonOBBBuffer constBufferConverterPrev;

};