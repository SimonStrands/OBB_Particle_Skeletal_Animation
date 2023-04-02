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

class AnimatedModel {
private:
	Joint jointHierarchy;
	//mesh
public:

};