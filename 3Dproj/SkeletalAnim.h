#ifndef SKELETALANIM_H
#define SKELETALANIM_H

#include <list>
#include <string>

#include "Animation.h"

struct Bone {
	std::vector<Bone> childJoints;
	//Bone* parent = nullptr;
	int id;
	std::string name;
	DirectX::XMMATRIX inverseBindPoseMatrix;
	DirectX::XMMATRIX boneMatrix;
	
};
#endif

//modified version and based on video link
//https://www.youtube.com/watch?v=f3Cr8Yx3GGA