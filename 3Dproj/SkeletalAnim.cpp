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

    inverseBindPoseMatrix = obj.inverseBindPoseMatrix;
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
//int Joint::GetId() const
//{
//    return this->id;
//}
//
//std::string Joint::GetName() const
//{
//    return this->name;
//}


XMMATRIX Joint::GetAnimatedTransform() const
{
    return this->animatedTransform;
}

void Joint::SetAnimationTransform(XMMATRIX animationTransform)
{
    this->animatedTransform = animatedTransform;
}

//XMMATRIX Joint::GetInverseBindTransform() const
//{
//    return this->inverseBindTransform;
//}

void Joint::CalcInverseBindTransform(XMMATRIX parentBindTransform)
{
	XMMATRIX bindTransform = XMMatrixMultiply(parentBindTransform, localBindTransform);
    
    inverseBindPoseMatrix = XMMatrixInverse(nullptr, bindTransform);
    for (Joint child : childJoints)
    {
        child.CalcInverseBindTransform(bindTransform);
    }

}



