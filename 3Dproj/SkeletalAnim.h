#pragma once
//#include <d3d11.h>
#include <DirectXMath.h>
#include <list>
#include <string>

class Joint {

private:
	std::list<Joint> childJoints;
	int id;
	std::string name;
	DirectX::XMMATRIX transform;
public:
	Joint();
};

class Mesh {
private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texCoords;
	DirectX::XMFLOAT3 normal;
	DirectX::XMINT3 jointIDs;
	DirectX::XMFLOAT3 weights;
public:
	Mesh();
};

class AnimatedModel {
private:
	Joint jointHierarchy;
	//mesh
public:

};

//https://www.youtube.com/watch?v=f3Cr8Yx3GGA