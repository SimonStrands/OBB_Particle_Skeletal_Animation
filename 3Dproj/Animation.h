#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <map>

class JointTransform {
private:
	DirectX::XMFLOAT3 position;
	DirectX::XMVECTOR rotation; //rotation in quaternion
public:
	JointTransform(DirectX::XMFLOAT3 pos, DirectX::XMVECTOR rot);
	JointTransform(DirectX::XMMATRIX localTransform);
	~JointTransform();

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMVECTOR GetRotation();
	DirectX::XMMATRIX GetLocalTransform();
	JointTransform Interpolate(JointTransform frameA, JointTransform frameB, float progression);
};

class KeyFrame {
private:
	std::map<std::string, JointTransform> jointKeyframes; //pose
	float timeStamp;
public:
	KeyFrame(float timeStamp, std::map<std::string, JointTransform> jointKeyframes);

	float GetTimeStamp();
	std::map<std::string, JointTransform> GetJointKeyFrames(); //pose

};

class Animation {
private:
	float length;
	std::vector<KeyFrame> keyFrames;
public:
	//Animation();
	Animation(float length, std::vector<KeyFrame> frames);


	float GetLength();
	std::vector<KeyFrame> GetKeyFrames();

};