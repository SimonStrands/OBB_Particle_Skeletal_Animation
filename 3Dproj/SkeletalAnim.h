#ifndef SKELETALANIMH
#define SKELETALANIMH

#include <cmath>
#include <vector>
#include <DirectXMath.h>
#include <list>
#include <string>
#include <map>
#include <unordered_map>

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


class Joint;
class AnimatedModel;


class Joint {

private:
	std::vector<Joint> childJoints;
	int id;
	std::string name;


public:
	Joint();
	Joint(int index, std::string name, DirectX::XMMATRIX bindLocalTransform);
	Joint(const Joint & obj);
	Joint operator=(const Joint& obj);
	DirectX::XMMATRIX localBindTransform;
	DirectX::XMMATRIX inverseBindTransform;
	DirectX::XMMATRIX animatedTransform;

	int GetId() const;
	std::string GetName() const;
	void addChild(Joint child);
	std::vector<Joint> GetChildJoints();
	

	DirectX::XMMATRIX GetAnimatedTransform() const;
	void SetAnimationTransform(DirectX::XMMATRIX animationTransform);
	//DirectX::XMMATRIX GetInverseBindTransform() const;
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
	
	Mesh mesh;
	//texture;
	Joint rootJoint;
	int jointCount;
	//Animator* animator;
public:
	AnimatedModel(Mesh model, /*Texture texture,*/ Joint rootJoint, int jointCount);
	
	Mesh GetMesh() const;
	//tEXTURE GetTexture();
	Joint GetRootJoint() const;
	//void DeleteProperties();


	//void DoAnimation(Animation animation);
	//void Update();

	std::vector<DirectX::XMMATRIX> GetJointTransforms();
	void AddJointsToArray(Joint headJoint, std::vector<DirectX::XMMATRIX> jointMatrices);

};



#endif

//[] get animator to work
//[] make joints loading for regular skeleton visible 

//modified version and based on video link
//https://www.youtube.com/watch?v=f3Cr8Yx3GGA