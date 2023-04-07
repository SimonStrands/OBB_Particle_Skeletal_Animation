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
std::vector<Joint*> Joint::GetChildJoints() const
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
