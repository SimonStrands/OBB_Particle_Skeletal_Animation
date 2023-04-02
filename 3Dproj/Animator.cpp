#include "Animator.h"

void Animator::incAnimationTime()
{
	this->animationTime += 0; // deltatime
	if (this->animationTime > currentAnim.GetLength())
	{
		this->animationTime = std::fmod(this->animationTime, currentAnim.GetLength());
	}
		
}

Animator::Animator()
{
}

Animator::~Animator()
{
}
