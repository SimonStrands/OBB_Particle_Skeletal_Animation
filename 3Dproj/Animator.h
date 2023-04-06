#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Animation.h"

#include <cmath>

class Joint;
class Mesh;
class AnimatedModel;

class Animator {
private:
	AnimatedModel* entity;
	Animation* currentAnim;
	float animationTime;

	std::map<std::string, DirectX::XMMATRIX> GetCurrAnimPose();
	void applyPoseToJoints(std::map<std::string, DirectX::XMMATRIX> currentPose, Joint* joint, DirectX::XMMATRIX parentTransform);
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

#endif