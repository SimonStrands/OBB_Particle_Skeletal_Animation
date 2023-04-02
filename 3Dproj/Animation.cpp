#include "Animation.h"


Animation::Animation(float length, std::vector<KeyFrame> frames)
{
    this->keyFrames = frames;
    this->length = length;
}

float Animation::GetLength()
{
    return this->length;
}

std::vector<KeyFrame> Animation::GetKeyFrames()
{
    return this->keyFrames;
}

KeyFrame::KeyFrame(float timeStamp, std::map<std::string, JointTransform> jointKeyframes)
{
    this->timeStamp = timeStamp;
    this->jointKeyframes = jointKeyframes;
}

float KeyFrame::GetTimeStamp()
{
    return this->timeStamp;
}

std::map<std::string, JointTransform> KeyFrame::GetJointKeyFrames()
{
    return this->jointKeyframes;
}

JointTransform::JointTransform(DirectX::XMFLOAT3 pos, DirectX::XMVECTOR rot)
{
    this->position = pos;
    this->rotation = rot;
}

JointTransform::JointTransform(DirectX::XMMATRIX localTransform)
{
    DirectX::XMFLOAT4X4 localTransform4x4;
    DirectX::XMStoreFloat4x4(&localTransform4x4, localTransform);
    this->position = DirectX::XMFLOAT3(localTransform4x4._31, localTransform4x4._32, localTransform4x4._33);
    this->rotation = DirectX::XMQuaternionRotationMatrix(localTransform);

}

JointTransform::~JointTransform()
{
}

DirectX::XMFLOAT3 JointTransform::GetPosition()
{
    return this->position;
}

DirectX::XMVECTOR JointTransform::GetRotation()
{
    return this->rotation;
}

DirectX::XMMATRIX JointTransform::GetLocalTransform()
{
    DirectX::XMMATRIX localtransform = DirectX::XMMatrixTranslation(this->position.x, this->position.y, this->position.z);
    localtransform = DirectX::XMMatrixMultiply(localtransform, DirectX::XMMatrixRotationQuaternion(this->rotation));
    return localtransform;
}

JointTransform JointTransform::Interpolate(JointTransform frameA, JointTransform frameB, float progression)
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMVECTOR posV = DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&frameA.GetPosition()), DirectX::XMLoadFloat3(&frameB.GetPosition()), progression);
    DirectX::XMStoreFloat3(&pos, posV);
    DirectX::XMVECTOR rot = DirectX::XMQuaternionSlerp(frameA.GetRotation(), frameB.GetRotation(), progression);
    return JointTransform(pos, rot);
}
