#ifndef SKELETALANIM_H
#define SKELETALANIM_H


#include <list>
#include <string>
#include <map>
#include <unordered_map>
#include <DirectXMath.h>

struct KeyFrame {
	std::vector<float> positionTimestamps;
	std::vector<float> rotationTimestamps;
	std::vector<float> scaleTimestamps;

	std::vector<DirectX::XMFLOAT3> positions = {};
	std::vector<DirectX::XMFLOAT4> rotations = {};
	std::vector<DirectX::XMFLOAT3> scales = {};
};

class Animation {
public:
	float length;
	float tick = 1.0f;
	std::unordered_map<std::string, KeyFrame> keyFrames;
};

class Joint {
public:
	Joint();
	Joint(int index, std::string name, DirectX::XMMATRIX bindLocalTransform);
	Joint(const Joint & obj);
	Joint operator=(const Joint& obj);

	std::vector<Joint> childJoints;
	int id;
	std::string name;
	DirectX::XMMATRIX localBindTransform;
	DirectX::XMMATRIX inverseBindPoseMatrix;
	//DirectX::XMMATRIX animatedTransform;

	//int GetId() const;
	//std::string GetName() const;
	void addChild(Joint child);
	std::vector<Joint> GetChildJoints();
	

	//DirectX::XMMATRIX GetAnimatedTransform() const;
	//void SetAnimationTransform(DirectX::XMMATRIX animationTransform);
	//DirectX::XMMATRIX GetInverseBindTransform() const;
	void CalcInverseBindTransform(DirectX::XMMATRIX parentBindTransform);
	
};

//struct Bone {
//	std::vector<Bone> childJoints;
//	int id;
//	std::string name;
//	DirectX::XMMATRIX inverseBindPoseMatrix;
//	DirectX::XMMATRIX boneMatrix;
//	
//
//};


#endif


//[] get animator to work
//[] make joints loading for regular skeleton visible 


//modified version and based on video link
//https://www.youtube.com/watch?v=f3Cr8Yx3GGA