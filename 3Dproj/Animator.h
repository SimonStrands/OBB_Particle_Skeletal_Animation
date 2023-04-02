#pragma once
#include "Animation.h"
#include <cmath>
class Animator {
private:

	Animation currentAnim;
	float animationTime;

	void incAnimationTime();

public:
	Animator();
	~Animator();


	void CalculateAnimationPose();


};