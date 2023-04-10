#include "SkeletalAnim.h"
#include "Animator.h"

Joint::Joint() //maybe make it impossible to make default joints? and only pointer to Joints?
{
    this->id = -1;
    this->name = "default";
    this->localBindTransform = DirectX::XMMATRIX();
}

Joint::Joint(int index, std::string name, DirectX::XMMATRIX bindLocalTransform)
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

void Joint::addChild(Joint* child)
{
    this->childJoints.push_back(child);
}
std::vector<Joint*> Joint::GetChildJoints()
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


DirectX::XMMATRIX Joint::GetAnimatedTransform() const
{
    return this->animatedTransform;
}

void Joint::SetAnimationTransform(DirectX::XMMATRIX animationTransform)
{
    this->animatedTransform = animatedTransform;
}

DirectX::XMMATRIX Joint::GetInverseBindTransform() const
{
    return this->inverseBindTransform;
}

void Joint::CalcInverseBindTransform(DirectX::XMMATRIX parentBindTransform)
{
    DirectX::XMMATRIX bindTransform = DirectX::XMMatrixMultiply(parentBindTransform, localBindTransform);
    
    inverseBindTransform = DirectX::XMMatrixInverse(nullptr, bindTransform);
    for (Joint* child : childJoints)
    {
        child->CalcInverseBindTransform(bindTransform);
    }

}

AnimatedModel::AnimatedModel(Mesh model, /*Texture texture,*/ Joint* rootJoint, int jointCount)
{
    this->mesh = model;
    //this->texture = texture;
    this->rootJoint = rootJoint;
    this->jointCount = jointCount;
    //this->animator = new Animator(this);
    rootJoint->CalcInverseBindTransform(DirectX::XMMATRIX());
}

Mesh AnimatedModel::GetMesh() const
{
    return this->mesh;
}

Joint* AnimatedModel::GetRootJoint() const
{
    return this->rootJoint;
}

void AnimatedModel::DoAnimation(Animation animation)
{
    animator->DoAnimation(animation);
}

void AnimatedModel::Update()
{
    animator->Update();
}

std::vector<DirectX::XMMATRIX> AnimatedModel::GetJointTransforms()
{
    std::vector<DirectX::XMMATRIX> jointMatrices;
    jointMatrices.resize(jointCount);
    AddJointsToArray(rootJoint, jointMatrices);
    return jointMatrices;
}

void AnimatedModel::AddJointsToArray(Joint* headJoint, std::vector<DirectX::XMMATRIX> jointMatrices)
{
    jointMatrices[headJoint->GetId()] = headJoint->GetAnimatedTransform();
    for (Joint* childJoint : headJoint->GetChildJoints()) {
        AddJointsToArray(childJoint, jointMatrices);
    }
}

//void AnimatedModel::DeleteProperties()
//{
//    //delete this->mesh
//}

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

using namespace DirectX;

Animation::Animation()
{
	this->length = 0;
	this->keyFrames.clear();
}

Animation::Animation(float length, std::vector<KeyFrame> frames)
{
    this->keyFrames = frames;
    this->length = length;
}

float Animation::GetLength()
{
    return this->length;
}

void Animation::SetLength(float length)
{
	this->length = length;
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

JointTransform::JointTransform(XMFLOAT3 pos, XMVECTOR rot)
{
    this->position = pos;
    this->rotation = rot;
}

JointTransform::JointTransform(XMMATRIX localTransform)
{
    XMFLOAT4X4 localTransform4x4;
    XMStoreFloat4x4(&localTransform4x4, localTransform);
    this->position = XMFLOAT3(localTransform4x4._31, localTransform4x4._32, localTransform4x4._33);
    this->rotation = XMQuaternionRotationMatrix(localTransform);

}

JointTransform::~JointTransform()
{
}

XMFLOAT3 JointTransform::GetPosition()
{
    return this->position;
}

XMVECTOR JointTransform::GetRotation()
{
    return this->rotation;
}

XMMATRIX JointTransform::GetLocalTransform()
{
    XMMATRIX localtransform = DirectX::XMMatrixTranslation(this->position.x, this->position.y, this->position.z);
    localtransform = DirectX::XMMatrixMultiply(localtransform, DirectX::XMMatrixRotationQuaternion(this->rotation));
    return localtransform;
}

JointTransform JointTransform::Interpolate(JointTransform frameA, JointTransform frameB, float progression)
{
    XMFLOAT3 pos;
    XMFLOAT3 posA = frameA.GetPosition();
    XMFLOAT3 posB = frameB.GetPosition();
    DirectX::XMVECTOR posV = DirectX::XMVectorLerp(XMLoadFloat3(&posA), XMLoadFloat3(&posB), progression);
    DirectX::XMStoreFloat3(&pos, posV);
    DirectX::XMVECTOR rot = DirectX::XMQuaternionSlerp(frameA.GetRotation(), frameB.GetRotation(), progression);
    return JointTransform(pos, rot);
}

std::map<std::string, DirectX::XMMATRIX> Animator::GetCurrAnimPose()
{
	std::vector<KeyFrame> frames = GetPreviousAndNextFrames();
	float progression = calculatProgression(frames[0], frames[1]);
	return calculateCurrentPose(frames[0], frames[1], progression);
}

void Animator::applyPoseToJoints(std::map<std::string, DirectX::XMMATRIX> currentPose, Joint* joint, DirectX::XMMATRIX parentTransform)
{
	DirectX::XMMATRIX currLocalTransform = currentPose.at(joint->GetName());
	DirectX::XMMATRIX currTransform = DirectX::XMMatrixMultiply(parentTransform, currLocalTransform);

	//for(int i<0;i<joint.get)
	for (Joint* childJoint : joint->GetChildJoints())
	{
		applyPoseToJoints(currentPose, childJoint, currTransform);

	}
	currTransform = DirectX::XMMatrixMultiply(currTransform, joint->GetInverseBindTransform());
	joint->SetAnimationTransform(currTransform);
}

std::vector<KeyFrame> Animator::GetPreviousAndNextFrames()
{
	KeyFrame* prevFrame = nullptr;
	KeyFrame* nextFrame = nullptr;
	for (KeyFrame frame : currentAnim->GetKeyFrames())
	{
		if (frame.GetTimeStamp() > animationTime)
		{
			nextFrame = &frame;
			break;
		}
		prevFrame = &frame;
	}
	if (prevFrame == nullptr)
	{
		prevFrame = nextFrame;
	}
	else if (nextFrame == nullptr)
	{
		nextFrame = prevFrame;
	}
	return std::vector<KeyFrame>(prevFrame, nextFrame);
}

float Animator::calculatProgression(KeyFrame previousFrame, KeyFrame nextFrame)
{
	float timeDiff = nextFrame.GetTimeStamp() - previousFrame.GetTimeStamp();
	return (this->animationTime - previousFrame.GetTimeStamp()) / timeDiff;
}

std::map<std::string, DirectX::XMMATRIX> Animator::calculateCurrentPose(KeyFrame previousFrame, KeyFrame nextFrame, float progression)
{
	std::map<std::string, DirectX::XMMATRIX> currentPose;// = hashmap

	std::vector<std::string> keys;
	for (auto it = previousFrame.GetJointKeyFrames().begin(); it != previousFrame.GetJointKeyFrames().end(); it++)
	{
		keys.push_back(it->first);
	}


	for (std::string jointName : keys)
	{
		JointTransform previousTransform = previousFrame.GetJointKeyFrames().at(jointName);
		JointTransform nextTransform = nextFrame.GetJointKeyFrames().at(jointName);
		JointTransform currentTransform = currentTransform.Interpolate(previousTransform, nextTransform, progression);
		currentPose.emplace(jointName, currentTransform.GetLocalTransform());
	}
	return currentPose;
}


void Animator::incAnimationTime()
{
	this->animationTime += 0; // deltatime
	if (this->animationTime > currentAnim->GetLength())
	{
		this->animationTime = std::fmod(this->animationTime, currentAnim->GetLength());
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

void Animator::Update()
{
	if (currentAnim == nullptr)
	{
		return;
	}
	incAnimationTime();
	std::map<std::string, DirectX::XMMATRIX> currentPose = GetCurrAnimPose();
	applyPoseToJoints(currentPose, entity->GetRootJoint(), DirectX::XMMATRIX());
}

void Animator::DoAnimation(Animation animation)
{
	this->animationTime = 0;
	this->currentAnim = &animation;
}