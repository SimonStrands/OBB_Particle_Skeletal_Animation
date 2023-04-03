#include "OBBSkeleton.h"

OBB::OBB(DirectX::XMFLOAT3 pos, DirectX::XMVECTOR rot, float height):
	height(height)
{
	this->position = pos;
	this->rotation = rot;
}

OBB::OBB(DirectX::XMMATRIX localTransform, float height):
	height(height)
{
	DirectX::XMFLOAT4X4 localTransform4x4;
    DirectX::XMStoreFloat4x4(&localTransform4x4, localTransform);
    this->position = DirectX::XMFLOAT3(localTransform4x4._31, localTransform4x4._32, localTransform4x4._33);
    this->rotation = DirectX::XMQuaternionRotationMatrix(localTransform);
}
