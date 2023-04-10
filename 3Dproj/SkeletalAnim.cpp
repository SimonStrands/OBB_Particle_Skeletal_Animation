#include "SkeletalAnim.h"

using namespace DirectX;

Joint::Joint() //maybe make it impossible to make default joints? and only pointer to Joints?
{
    this->id = -1;
    this->name = "default";
    this->localBindTransform = XMMATRIX();
}

Joint::Joint(int index, std::string name, XMMATRIX bindLocalTransform)
{
    this->id = index;
    this->name = name;
    this->localBindTransform = bindLocalTransform;

}

Joint::Joint(const Joint& obj)
{
    childJoints = obj.childJoints;

    id = obj.id;
    name = obj.name;
    localBindTransform = obj.localBindTransform;

    inverseBindTransform = obj.inverseBindTransform;
    animatedTransform = obj.animatedTransform;
}

Joint Joint::operator=(const Joint& obj)
{

	Joint temp(obj.id, obj.name, obj.localBindTransform);
	return temp;
}

void Joint::addChild(Joint child)
{
    this->childJoints.push_back(child);
}
std::vector<Joint> Joint::GetChildJoints()
{
    return this->childJoints;
}
int Joint::GetId() const
{
    return this->id;
}

std::string Joint::GetName() const
{
    return this->name;
}


XMMATRIX Joint::GetAnimatedTransform() const
{
    return this->animatedTransform;
}

void Joint::SetAnimationTransform(XMMATRIX animationTransform)
{
    this->animatedTransform = animatedTransform;
}

XMMATRIX Joint::GetInverseBindTransform() const
{
    return this->inverseBindTransform;
}

void Joint::CalcInverseBindTransform(XMMATRIX parentBindTransform)
{
	XMMATRIX bindTransform = XMMatrixMultiply(parentBindTransform, localBindTransform);
    
    inverseBindTransform = XMMatrixInverse(nullptr, bindTransform);
    for (Joint child : childJoints)
    {
        child.CalcInverseBindTransform(bindTransform);
    }

}

AnimatedModel::AnimatedModel(Mesh model, /*Texture texture,*/ Joint rootJoint, int jointCount)
{
    this->mesh = model;
    //this->texture = texture;
    this->rootJoint = rootJoint;
    this->jointCount = jointCount;
    //this->animator = new Animator(this);
    rootJoint.CalcInverseBindTransform(XMMATRIX());
}

Mesh AnimatedModel::GetMesh() const
{
    return this->mesh;
}

Joint AnimatedModel::GetRootJoint() const
{
    return this->rootJoint;
}

void AnimatedModel::DoAnimation(Animation animation)
{
    animator->DoAnimation(animation);
}

void AnimatedModel::Update()
{
    //animator->Update();
}

std::vector<XMMATRIX> AnimatedModel::GetJointTransforms()
{
    std::vector<XMMATRIX> jointMatrices;
    jointMatrices.resize(jointCount);
    AddJointsToArray(rootJoint, jointMatrices);
    return jointMatrices;
}

void AnimatedModel::AddJointsToArray(Joint headJoint, std::vector<XMMATRIX> jointMatrices)
{
    jointMatrices[headJoint.GetId()] = headJoint.GetAnimatedTransform();
    for (Joint childJoint : headJoint.GetChildJoints()) {
        AddJointsToArray(childJoint, jointMatrices);
    }
}

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}



//std::map<std::string, XMMATRIX> Animator::GetCurrAnimPose()
//{
//	std::vector<KeyFrame> frames = GetPreviousAndNextFrames();
//	float progression = calculatProgression(frames[0], frames[1]);
//	return calculateCurrentPose(frames[0], frames[1], progression);
//}

void Animator::applyPoseToJoints(std::map<std::string, XMMATRIX> currentPose, Joint joint, XMMATRIX parentTransform)
{
	XMMATRIX currLocalTransform = currentPose.at(joint.GetName());
	XMMATRIX currTransform = XMMatrixMultiply(parentTransform, currLocalTransform);

	for (Joint childJoint : joint.GetChildJoints())
	{
		applyPoseToJoints(currentPose, childJoint, currTransform);

	}
	currTransform = XMMatrixMultiply(currTransform, joint.GetInverseBindTransform());
	joint.SetAnimationTransform(currTransform);
}

//std::vector<KeyFrame> Animator::GetPreviousAndNextFrames()
//{
//	KeyFrame* prevFrame = nullptr;
//	KeyFrame* nextFrame = nullptr;
//	for (KeyFrame frame : currentAnim->keyFrames)
//	{
//		if (frame.GetTimeStamp() > animationTime)
//		{
//			nextFrame = &frame;
//			break;
//		}
//		prevFrame = &frame;
//	}
//	if (prevFrame == nullptr)
//	{
//		prevFrame = nextFrame;
//	}
//	else if (nextFrame == nullptr)
//	{
//		nextFrame = prevFrame;
//	}
//	return std::vector<KeyFrame>(prevFrame, nextFrame);
//}

float Animator::calculatProgression(float previousFrame, float nextFrame)
{
	float timeDiff = nextFrame - previousFrame;
	return (this->animationTime - previousFrame) / timeDiff;
}

//std::map<std::string, XMMATRIX> Animator::calculateCurrentPose(KeyFrame previousFrame, KeyFrame nextFrame, float progression)
//{
//	std::map<std::string, XMMATRIX> currentPose;// = hashmap
//
//	std::vector<std::string> keys;
//	for (auto it = previousFrame.GetJointKeyFrames().begin(); it != previousFrame.GetJointKeyFrames().end(); it++)
//	{
//		keys.push_back(it->first);
//	}
//
//
//	for (std::string jointName : keys)
//	{ 
//		//use keyframes positin instead
//		JointTransform previousTransform = previousFrame.GetJointKeyFrames().at(jointName);
//		JointTransform nextTransform = nextFrame.GetJointKeyFrames().at(jointName);
//		JointTransform currentTransform = currentTransform.Interpolate(previousTransform, nextTransform, progression);
//		currentPose.emplace(jointName, currentTransform.GetLocalTransform());
//	}
//	return currentPose;
//}


void Animator::incAnimationTime()
{
	this->animationTime += 0; // deltatime
	if (this->animationTime > currentAnim->length)
	{
		this->animationTime = std::fmod(this->animationTime, currentAnim->length);
	}

}

Animator::Animator()
{
	this->entity = nullptr;
	this->currentAnim = nullptr;
	this->animationTime = 0.f;
}

Animator::Animator(AnimatedModel* entity)
{
	*this->entity = *entity;
	this->currentAnim = nullptr;
	this->animationTime = 0.f;
}

Animator::Animator(const Animator& obj)
{
	if (obj.entity != nullptr)
		*this->entity = *obj.entity;
	if (obj.currentAnim != nullptr)
		*this->currentAnim = *obj.currentAnim;
	this->animationTime = obj.animationTime;
}

Animator::~Animator()
{
}

//void Animator::Update()
//{
//	if (currentAnim == nullptr)
//	{
//		return;
//	}
//	incAnimationTime();
//	std::map<std::string, XMMATRIX> currentPose = GetCurrAnimPose();
//	applyPoseToJoints(currentPose, entity->GetRootJoint(), XMMATRIX());
//}

void Animator::DoAnimation(Animation animation)
{
	this->animationTime = 0;
	this->currentAnim = &animation;
}