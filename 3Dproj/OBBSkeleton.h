#pragma once
#include "Vec.h"

//This is probably not how it should look in final build
class OBB{
public:
	OBB(DirectX::XMFLOAT3 pos, DirectX::XMVECTOR rot, float height);
	OBB(DirectX::XMMATRIX localTransform, float height);
private:
	DirectX::XMFLOAT3 position;
	DirectX::XMVECTOR rotation; //rotation in quaternion
	const float width = 20;
	const float depth = 20;
	const float height;
};