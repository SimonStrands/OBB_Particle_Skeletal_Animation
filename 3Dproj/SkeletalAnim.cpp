#include "SkeletalAnim.h"

Joint::Joint() //maybe make it impossible to make default joints? and only pointer to Joints?
{
    this->id = -1;
    this->name = "default";
}

Joint::Joint(int index, std::string name, DirectX::XMMATRIX bindLocalTransform)
{
    this->id = index;
    this->name = name;

}

Joint::Joint(const Joint& obj)
{
    childJoints = obj.childJoints;

    id = obj.id;
    name = obj.name;
    offset = obj.offset;
}

void Joint::addChild(Joint child)
{
    this->childJoints.push_back(child);
}
std::list<Joint> Joint::GetChildJoints() const
{
    return this->childJoints;
}
DirectX::XMMATRIX& Joint::getOffsetMatrix()
{
    return this->offset;
}
int& Joint::GetId()
{
    return this->id;
}

std::string& Joint::GetName()
{
    return this->name;
}



//void AnimatedModel::DeleteProperties()
//{
//    //delete this->mesh
//}


