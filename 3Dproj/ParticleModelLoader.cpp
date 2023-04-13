#include "ParticleModelLoader.h"
#include <iostream>

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

void Nodes(int & nrTotal, Joint & parent, aiNode* walker)
{
	std::string name = walker->mName.C_Str();
	DirectX::XMMATRIX mat = AiMatrixToXMMATRIX(walker->mTransformation);
	
	for (int i = 0; i < walker->mNumChildren; i++)
	{
		nrTotal++;
		aiNode* temp = walker->mChildren[i]; //get info to create child node
		std::string name = temp->mName.C_Str();
		DirectX::XMMATRIX mat = AiMatrixToXMMATRIX(temp->mTransformation);
		Joint childJoint = Joint(nrTotal, name, mat);

		parent.addChild(childJoint);
		Joint nextJoint = parent.GetChildJoints()[i]; //traverse
		Nodes(nrTotal, nextJoint, temp);
	}
	
}


void testReadHiaechy(aiNode *pNode, const aiMatrix4x4& parentMatrix, std::vector<DirectX::XMMATRIX> &Transformations)
{
	aiMatrix4x4 newParentMatrix = parentMatrix * pNode->mTransformation;
	aiMatrix4x4 m = newParentMatrix;
				Transformations.push_back(DirectX::XMMATRIX(
					m.a1, m.b1, m.c1, m.d1,
					m.a2, m.b2, m.c2, m.d2,
					m.a3, m.b3, m.c3, m.d3,
					m.a4*0.01f, m.b4*0.01f, m.c4*0.01f, m.d4
				));
	for(int i = 0; i < pNode->mNumChildren; i++){
		testReadHiaechy(pNode->mChildren[i], newParentMatrix, Transformations);
	}
}

bool readSkeleton(std::unordered_map<std::string, std::pair<int, DirectX::XMMATRIX>> boneInfo, Joint& joint, aiNode* node){

	if (boneInfo.find(node->mName.C_Str()) != boneInfo.end()) { // if node is actually a bone
		joint.name = node->mName.C_Str();
		joint.id = boneInfo[joint.name].first;
		//joint.inverseBindPoseMatrix = boneInfo[joint.name].second;
		joint.inverseBindPoseMatrix = DirectX::XMMatrixTranspose(boneInfo[joint.name].second);
		//joint.inverseBindPoseMatrix = DirectX::XMMatrixInverse(nullptr, boneInfo[joint.name].second);
	
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			Joint child;
			if(readSkeleton(boneInfo, child, node->mChildren[i])){
				joint.childJoints.push_back(child);
			}
		}
		return true;
	}
	else { // find bones in children
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			if (readSkeleton(boneInfo, joint, node->mChildren[i])) {
				return true;
			}
	
		}
	}
	return false;
}

#include <iostream>

void loadBoneDataToVertecies(
	std::vector<VolumetricVertex>& vertecies, 
	Joint& rootJoint, 
	aiMesh* mesh,
	aiNode* node,
	int verteciesSize
){
	std::unordered_map<std::string, std::pair<int, DirectX::XMMATRIX>> boneInfo = {};
	std::vector<uint16_t> boneCounts;
	boneCounts.resize(verteciesSize);

	for(unsigned int i = 0; i < mesh->mNumBones; i++){
		aiBone* bone = mesh->mBones[i];
		DirectX::XMMATRIX m = AiMatrixToXMMATRIX(bone->mOffsetMatrix);
		boneInfo[bone->mName.C_Str()] = {i, m};
		#ifdef TRADITIONALSKELETALANIMATION
		//loop through each vertex that have that bone
		for (unsigned int j = 0; j < bone->mNumWeights; j++) {
			unsigned int id = bone->mWeights[j].mVertexId;
			float weight = bone->mWeights[j].mWeight;
			boneCounts[id]++;
			switch (boneCounts[id]) {
			case 1:
				vertecies[id].boneIDs[0] = i;
				vertecies[id].boneWeights[0] = weight;
				break;
			case 2:
				vertecies[id].boneIDs[1] = i;
				vertecies[id].boneWeights[1] = weight;
				break;
			case 3:
				vertecies[id].boneIDs[2] = i;
				vertecies[id].boneWeights[2] = weight;
				break;
			case 4:
				vertecies[id].boneIDs[3] = i;
				vertecies[id].boneWeights[3] = weight;
				break;
			default:

				break;

			}
		}
		//normalize weights to make all weights sum 1
		for (int i = 0; i < vertecies.size(); i++) {
			
			float totalWeight = vertecies[i].boneWeights[0]
				+ vertecies[i].boneWeights[1]
				+ vertecies[i].boneWeights[2]
				+ vertecies[i].boneWeights[3];

			if (totalWeight > 0.0f) {
				vertecies[i].boneWeights[0] = vertecies[i].boneWeights[0] / totalWeight;
				vertecies[i].boneWeights[1] = vertecies[i].boneWeights[1] / totalWeight;
				vertecies[i].boneWeights[2] = vertecies[i].boneWeights[2] / totalWeight;
				vertecies[i].boneWeights[3] = vertecies[i].boneWeights[3] / totalWeight;
			}
		}
		#endif 
	}
	readSkeleton(boneInfo, rootJoint, node);
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
	animation.length = (float)(anim->mDuration);
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
void loadParticleModel(std::vector<VolumetricVertex>& vertecies, const std::string& filePath, Animation& animation, DirectX::XMMATRIX &globalInverseTransform, Joint& rootJoint)
{
	Assimp::Importer AImporter;
	const aiScene* scene = AImporter.ReadFile(filePath, aiProcess_JoinIdenticalVertices);

	//exit if no scene/file was found
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		std::cout << "ERROR could find scene or file" << std::endl;
		exit(-2);
	}

	//FOR Now just do the first one
	aiMesh* mesh = scene->mMeshes[0];
	uint16_t boneCount = 0;
	
	//load mesh/particle form
	for(unsigned int v = 0; v < mesh->mNumVertices; v++){
		aiVector3D vertex = mesh->mVertices[v];
		vertecies.push_back(VolumetricVertex(vertex.x, vertex.y, vertex.z, 0, 0, 1, 0.75f));
	}
	if (mesh->HasBones()) 
	{
		int totalnr = 0;

		std::string name = scene->mRootNode->mName.C_Str();
		DirectX::XMMATRIX mat = AiMatrixToXMMATRIX(scene->mRootNode->mTransformation);
		Joint newRoot = Joint(totalnr, name, mat);
		
		globalInverseTransform = DirectX::XMMatrixInverse(nullptr, mat);

		/*Nodes(totalnr, newRoot, scene->mRootNode);
		newRoot.CalcInverseBindTransform(newRoot.localBindTransform);
		rootJoint = newRoot;*/
	}
	

	//load Bones
	loadBoneDataToVertecies(vertecies, rootJoint, mesh, scene->mRootNode, vertecies.size());

	//load Animation
	loadAnimation(scene, animation);
	
}

