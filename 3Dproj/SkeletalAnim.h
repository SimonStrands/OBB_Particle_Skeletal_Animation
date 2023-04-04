#pragma once
#include <DirectXMath.h>
#include <list>
#include <string>

#include "Animator.h"

class Joint {

private:
	std::list<Joint> childJoints;
	int id;
	std::string name;
	DirectX::XMMATRIX localBindTransform;

	DirectX::XMMATRIX inverseBindTransform;
	DirectX::XMMATRIX animatedTransform;
public:
	Joint();
	Joint(int index, std::string name, DirectX::XMMATRIX bindLocalTransform);
	Joint(const Joint & obj);


	int GetId();
	std::string GetName();
	void addChild(Joint child);
	std::list<Joint> GetChildJoints();
	
	DirectX::XMMATRIX GetAnimatedTransform();
	void SetAnimationTransform(DirectX::XMMATRIX animationTransform);
	DirectX::XMMATRIX GetInverseBindTransform();
	void CalcInverseBindTransform(DirectX::XMMATRIX parentBindTransform);
	
};

class Mesh {
private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texCoords; //
	DirectX::XMFLOAT3 normal; //
	DirectX::XMINT3 jointIDs;
	DirectX::XMFLOAT3 weights;
public:
	Mesh();


};

class AnimatedModel {
private:
	Joint jointHierarchy;
	Mesh mesh;
	//texture;
	Joint rootJoint;
	int jointCount;
	Animator animator;
public:
	AnimatedModel(Mesh model, /*Texture texture,*/ Joint rootJoint, int jointCount);
	
	Mesh GetMesh();
	//tEXTURE GetTexture();
	Joint GetRootJoint();
	//void DeleteProperties();


	void DoAnimation(Animation animation);
	void Update();

	std::vector<DirectX::XMMATRIX> GetJointTransforms();
	void AddJointsToArray(Joint headJoint, std::vector<DirectX::XMMATRIX> jointMatrices);

};


//modified version and based on video link
//https://www.youtube.com/watch?v=f3Cr8Yx3GGA