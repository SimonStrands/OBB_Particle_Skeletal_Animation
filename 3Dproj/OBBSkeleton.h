#pragma once
#include "Vec.h"
#include <vector>
#include "Graphics.h"
#include "point.h"

struct OBBSkeletonOBBBuffer : CB{
	struct{
		DirectX::XMMATRIX element[50];
	}transform;
	struct{
		DirectX::XMMATRIX element;
	}view;
	struct{
		DirectX::XMMATRIX element;
	}projection;
};

static const float OBBWidth = 1;
static const float OBBDepth = 1;

class OBBSkeletonDebug{
public:
	OBBSkeletonDebug(std::vector<DirectX::XMMATRIX>& transform, std::vector<float>& height, Graphics*& gfx);
	~OBBSkeletonDebug();
	void setTransformations(std::vector<DirectX::XMMATRIX>& transform);
	void draw(Graphics*& gfx);

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