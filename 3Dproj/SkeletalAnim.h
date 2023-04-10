#ifndef SKELETALANIMH
#define SKELETALANIMH

#include <cmath>
#include <vector>
#include <DirectXMath.h>
#include <list>
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
	Animation();
	Animation(float length, std::vector<KeyFrame> frames);


	float GetLength();
	void SetLength(float length);
	std::vector<KeyFrame> GetKeyFrames();

};
class Joint;
class AnimatedModel;
class Animator {
private:
	AnimatedModel* entity;
	Animation* currentAnim;
	float animationTime;

	std::map<std::string, DirectX::XMMATRIX> GetCurrAnimPose();
	void applyPoseToJoints(std::map<std::string, DirectX::XMMATRIX> currentPose, Joint joint, DirectX::XMMATRIX parentTransform);
	std::vector<KeyFrame> GetPreviousAndNextFrames();
	float calculatProgression(KeyFrame previousFrame, KeyFrame nextFrame);
	std::map<std::string, DirectX::XMMATRIX> calculateCurrentPose(KeyFrame previousFrame, KeyFrame nextFrame, float progression);
	void incAnimationTime();


public:
	Animator();
	Animator(AnimatedModel* entity);
	Animator(const Animator& obj);
	~Animator();


	void Update();
	void DoAnimation(Animation animation);

};


class Joint {

private:
	std::vector<Joint> childJoints;
	int id;
	std::string name;
	

	DirectX::XMMATRIX inverseBindTransform;
	DirectX::XMMATRIX animatedTransform;
public:
	Joint();
	Joint(int index, std::string name, DirectX::XMMATRIX bindLocalTransform);
	Joint(const Joint & obj);
	Joint operator=(const Joint& obj);
	DirectX::XMMATRIX localBindTransform;

	int GetId() const;
	std::string GetName() const;
	void addChild(Joint child);
	std::vector<Joint> GetChildJoints();
	

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
	
	Mesh mesh;
	//texture;
	Joint rootJoint;
	int jointCount;
	Animator* animator;
public:
	AnimatedModel(Mesh model, /*Texture texture,*/ Joint rootJoint, int jointCount);
	
	Mesh GetMesh() const;
	//tEXTURE GetTexture();
	Joint GetRootJoint() const;
	//void DeleteProperties();


	void DoAnimation(Animation animation);
	void Update();

	std::vector<DirectX::XMMATRIX> GetJointTransforms();
	void AddJointsToArray(Joint headJoint, std::vector<DirectX::XMMATRIX> jointMatrices);

};







#endif



//modified version and based on video link
//https://www.youtube.com/watch?v=f3Cr8Yx3GGA