#ifndef SKELETALANIM_H
#define SKELETALANIM_H

#include <list>
#include <string>

#include "Animator.h"

class Joint {

private:
	std::vector<Joint> childJoints;
	int id;
	std::string name;
	DirectX::XMMATRIX offset;
public:
	Joint();
	Joint(int index, std::string name, DirectX::XMMATRIX bindLocalTransform);
	Joint(const Joint & obj);


	int &GetId();
	std::string &GetName();
	void addChild(Joint child);
	std::vector<Joint> GetChildJoints() const;
	
	DirectX::XMMATRIX& getOffsetMatrix();
	
};

//
//class AnimatedModel {
//private:
//	Joint jointHierarchy;
//	Mesh mesh;
//	//texture;
//	Joint* rootJoint;
//	int jointCount;
//	Animator* animator;
//public:
//	AnimatedModel(Mesh model, /*Texture texture,*/ Joint rootJoint, int jointCount);
//	
//	Mesh GetMesh() const;
//	//tEXTURE GetTexture();
//	Joint* GetRootJoint() const;
//	//void DeleteProperties();
//
//
//	void DoAnimation(Animation animation);
//	void Update();
//
//	std::vector<DirectX::XMMATRIX> GetJointTransforms();
//	void AddJointsToArray(Joint* headJoint, std::vector<DirectX::XMMATRIX> jointMatrices);
//
//};
#endif

//modified version and based on video link
//https://www.youtube.com/watch?v=f3Cr8Yx3GGA