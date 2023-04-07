#ifndef SKELETALANIM_H
#define SKELETALANIM_H

#include <vector>
#include <DirectXMath.h>
#include <list>
#include <string>

class Animation;
class Animator;

class Joint {

private:
	std::vector<Joint*> childJoints;
	int id;
	std::string name;
	DirectX::XMMATRIX localBindTransform;

	DirectX::XMMATRIX inverseBindTransform;
	DirectX::XMMATRIX animatedTransform;
public:
	Joint();
	Joint(int index, std::string name, DirectX::XMMATRIX bindLocalTransform);
	Joint(const Joint & obj);


	int GetId() const;
	std::string GetName() const;
	void addChild(Joint* child);
	std::vector<Joint*> GetChildJoints() const;
	
	DirectX::XMMATRIX GetAnimatedTransform() const;
	void SetAnimationTransform(DirectX::XMMATRIX animationTransform);
	DirectX::XMMATRIX GetInverseBindTransform() const;
	void CalcInverseBindTransform(DirectX::XMMATRIX parentBindTransform);
	
};

//vertex
class Mesh {
private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texCoords; //
	DirectX::XMFLOAT3 normal; //
	DirectX::XMINT3 jointIDs;
	DirectX::XMFLOAT3 weights;
public:
	Mesh();
	~Mesh();


};

class AnimatedModel {
private:
	//Joint* jointHierarchy;
	Mesh mesh;
	//texture;
	Joint* rootJoint;
	int jointCount;
	Animator* animator;
public:
	AnimatedModel(Mesh model, /*Texture texture,*/ Joint* rootJoint, int jointCount);
	
	Mesh GetMesh() const;
	//tEXTURE GetTexture();
	Joint* GetRootJoint() const;
	//void DeleteProperties();


	void DoAnimation(Animation animation);
	void Update();

	std::vector<DirectX::XMMATRIX> GetJointTransforms();
	void AddJointsToArray(Joint* headJoint, std::vector<DirectX::XMMATRIX> jointMatrices);

};
#endif



//modified version and based on video link
//https://www.youtube.com/watch?v=f3Cr8Yx3GGA