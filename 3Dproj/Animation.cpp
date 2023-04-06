#include "Animation.h"

//using namespace DirectX;

//Animation::Animation(float length, std::vector<KeyFrame> frames)
//{
//    this->keyFrames = frames;
//    this->length = length;
//}
//
//float Animation::GetLength()
//{
//    return this->length;
//}
//
//std::vector<KeyFrame> Animation::GetKeyFrames()
//{
//    return this->keyFrames;
//}
//
//KeyFrame::KeyFrame(float timeStamp, std::map<std::string, JointTransform> jointKeyframes)
//{
//    this->timeStamp = timeStamp;
//    this->jointKeyframes = jointKeyframes;
//}
//
//float KeyFrame::GetTimeStamp()
//{
//    return this->timeStamp;
//}
//
//std::map<std::string, JointTransform> KeyFrame::GetJointKeyFrames()
//{
//    return this->jointKeyframes;
//}
//
//JointTransform::JointTransform(XMFLOAT3 pos, XMVECTOR rot)
//{
//    this->position = pos;
//    this->rotation = rot;
//}
//
//JointTransform::JointTransform(XMMATRIX localTransform)
//{
//    XMFLOAT4X4 localTransform4x4;
//    XMStoreFloat4x4(&localTransform4x4, localTransform);
//    this->position = XMFLOAT3(localTransform4x4._31, localTransform4x4._32, localTransform4x4._33);
//    this->rotation = XMQuaternionRotationMatrix(localTransform);
//
//}
//
//JointTransform::~JointTransform()
//{
//}
//
//XMFLOAT3 JointTransform::GetPosition()
//{
//    return this->position;
//}
//
//XMVECTOR JointTransform::GetRotation()
//{
//    return this->rotation;
//}
//
//DirectX::XMFLOAT3 JointTransform::GetScale()
//{
//    return this->scale;
//}
//
//XMMATRIX JointTransform::GetLocalTransform()
//{
//    XMMATRIX localtransform = DirectX::XMMatrixTranslation(this->position.x, this->position.y, this->position.z);
//    localtransform = DirectX::XMMatrixMultiply(localtransform, DirectX::XMMatrixRotationQuaternion(this->rotation));
//    return localtransform;
//}
//
//JointTransform JointTransform::Interpolate(JointTransform frameA, JointTransform frameB, float progression)
//{
//    XMFLOAT3 pos;
//    XMFLOAT3 posA = frameA.GetPosition(); 
//    XMFLOAT3 posB = frameB.GetPosition();
//    DirectX::XMVECTOR posV = DirectX::XMVectorLerp(XMLoadFloat3(&posA), XMLoadFloat3(&posB), progression);
//    DirectX::XMStoreFloat3(&pos, posV);
//    DirectX::XMVECTOR rot = DirectX::XMQuaternionSlerp(frameA.GetRotation(), frameB.GetRotation(), progression);
//    return JointTransform(pos, rot);
//}
