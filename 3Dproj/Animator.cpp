#include "Animator.h"

std::map<std::string, DirectX::XMMATRIX> Animator::GetCurrAnimPose()
{
	std::vector<KeyFrame> frames = GetPreviousAndNextFrames();
	float progression = calculatProgression(frames[0], frames[1]);
	return calculateCurrentPose(frames[0], frames[1], progression);
}

void Animator::applyPoseToJoints(std::map<std::string, DirectX::XMMATRIX> currentPose, Joint joint, DirectX::XMMATRIX parentTransform)
{
	DirectX::XMMATRIX currLocalTransform = currentPose.at(joint.GetName());
	DirectX::XMMATRIX currTransform = DirectX::XMMatrixMultiply(parentTransform, currLocalTransform);
	for (Joint childJoint : joint.GetChildJoints())
	{
		applyPoseToJoints(currentPose, childJoint, currTransform);

	}
	currTransform= DirectX::XMMatrixMultiply(currTransform, joint.GetInverseBindTransform());
	joint.SetAnimationTransform(currTransform);
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
	return std::vector<KeyFrame>(prevFrame,nextFrame);
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


	for (std::string jointname : keys)
	{
		JointTransform previousPose = previousFrame.GetJointKeyFrames().at(jointname);
		JointTransform nextPose = nextFrame.GetJointKeyFrames().at(jointname);
	}
	return std::map<std::string, DirectX::XMMATRIX>();
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
	applyPoseToJoints(currentPose, entity.GetRootJoints()), rotations);
}


