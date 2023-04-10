#include "ParticleModelLoader.h"

DirectX::XMFLOAT3 AiVector3ToXMFLOAT3(const aiVector3D& vec3) {
	return DirectX::XMFLOAT3(vec3.x, vec3.y, vec3.z);
}
DirectX::XMFLOAT4 AiQuadToXMFLOAT4(const aiQuaternion& quad) {
	//return DirectX::XMFLOAT4(quad.x,quad.y,quad.z, quad.w);
	return DirectX::XMFLOAT4(quad.x, quad.y, quad.z, quad.w);
}

DirectX::XMMATRIX AiMatrixToXMMATRIX(aiMatrix4x4 mat)
{
	DirectX::XMFLOAT4X4 mat4;
	mat4._11 = mat.a1;
	mat4._12 = mat.b1;
	mat4._13 = mat.c1;
	mat4._14 = mat.d1;
	mat4._21 = mat.a2;
	mat4._22 = mat.b2;
	mat4._23 = mat.c2;
	mat4._24 = mat.d2;
	mat4._31 = mat.a3;
	mat4._32 = mat.b3;
	mat4._33 = mat.c3;
	mat4._34 = mat.d3;
	mat4._41 = mat.a4;
	mat4._42 = mat.b4;
	mat4._43 = mat.c4;
	mat4._44 = mat.d4;
	DirectX::XMMATRIX xmat = DirectX::XMLoadFloat4x4(&mat4);
	return xmat;
}
void Nodes(int & nrTotal, Joint parent, aiNode* walker)
{
	std::string name = walker->mName.C_Str();
	DirectX::XMMATRIX mat = AiMatrixToXMMATRIX(walker->mTransformation);
	

	for (int i = 0; i < walker->mNumChildren; i++)
	{
		nrTotal++;

		aiNode* temp = walker->mChildren[i];

		std::string name = temp->mName.C_Str();
		DirectX::XMMATRIX mat = AiMatrixToXMMATRIX(temp->mTransformation);
		Joint childJoint = Joint(nrTotal, name, mat);

		parent.addChild(childJoint);
		Joint nextJoint = parent.GetChildJoints()[i]; //traverse

		Nodes(nrTotal, nextJoint, temp);
	}
	
	
}
void loadParticleModel(std::vector<VolumetricVertex>& vertecies, const std::string& filePath, Animation& animation, Joint & rootJoint)
{
	Assimp::Importer AImporter;
	const aiScene* scene = AImporter.ReadFile(filePath, aiProcess_JoinIdenticalVertices);
	loadAnimation(scene, animation);
	
	for(unsigned int i = 0; i < scene->mNumMeshes; i++){
		
		for (unsigned int v = 0; v < scene->mMeshes[i]->mNumVertices; v++) {
			aiVector3D vertex = scene->mMeshes[i]->mVertices[v];
			vertecies.push_back(VolumetricVertex(vertex.x, vertex.y, vertex.z, 0, 0, 1, 0.75f));
		}
		if (scene->HasAnimations())
		{
			
		}

		if(scene->mMeshes[i]->HasBones()){

			//load animations and bones
			//need:
			//name
			//bone transformation
			//inverse?
			//scene->mMeshes[i].
			//scene->mMeshes[i].

			//scene->mR
		
			int totalnr = 0;
			aiNode* walker = scene->mRootNode; //pointer to current node ->mChildren[0]->mTransformation

			std::string name = walker->mName.C_Str();
			DirectX::XMMATRIX mat = AiMatrixToXMMATRIX(walker->mTransformation);
			Joint newRoot = Joint(totalnr, name, mat);

			Nodes(totalnr, newRoot, walker);
			rootJoint = newRoot;
			
	
		}
		
	}
	
}

void loadSkeletalModel(Joint*& root, const std::string& filePath)
{

	Assimp::Importer AImporter;
	const aiScene* scene = AImporter.ReadFile(filePath, aiProcess_JoinIdenticalVertices);
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) 
	{
		if (scene->mMeshes[i]->HasBones()) 
		{
			int totalnr = 0;
			aiNode* walker = scene->mRootNode; //pointer to current node ->mChildren[0]->mTransformation

			std::string name = walker->mName.C_Str();
			DirectX::XMMATRIX mat = AiMatrixToXMMATRIX(walker->mTransformation);
			Joint rootJoint = Joint(totalnr, name, mat);

			Nodes(totalnr,rootJoint, walker);
		

		}
	}
}

bool loadAnimation(const aiScene* scene, Animation& animation)
{
	aiAnimation* anim = scene->mAnimations[0];

	if (anim->mTicksPerSecond != 0.0f) {
		animation.tick = (float)anim->mTicksPerSecond;
	}
	else {
		animation.tick = (float)anim->mTicksPerSecond;
	}
	animation.length = (float)(anim->mDuration * anim->mTicksPerSecond);
	animation.keyFrames = {};

	for (unsigned int i = 0; i < anim->mNumChannels; i++) {
		aiNodeAnim* channel = anim->mChannels[i];
		KeyFrame track;
		for (unsigned int j = 0; j < channel->mNumPositionKeys; j++) {
			track.positionTimestamps.push_back((float)channel->mPositionKeys[j].mTime);
			track.positions.push_back(AiVector3ToXMFLOAT3(channel->mPositionKeys[j].mValue));
		}
		for (unsigned int j = 0; j < channel->mNumRotationKeys; j++) {
			track.rotationTimestamps.push_back((float)channel->mRotationKeys[j].mTime);
			track.rotations.push_back(AiQuadToXMFLOAT4(channel->mRotationKeys[j].mValue));

		}
		for (unsigned int j = 0; j < channel->mNumScalingKeys; j++) {
			track.scaleTimestamps.push_back((float)channel->mScalingKeys[j].mTime);
			track.scales.push_back(AiVector3ToXMFLOAT3(channel->mScalingKeys[j].mValue));

		}
		animation.keyFrames[channel->mNodeName.C_Str()] = track;
	}
	return true;
}
