#include "ParticleModelLoader.h"
#include <iostream>
#include "Random.h"

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
DirectX::XMFLOAT3 AiVector3ToXMFLOAT3(const aiVector3D& vec3){
	return DirectX::XMFLOAT3(vec3.x,vec3.y,vec3.z);
}
DirectX::XMFLOAT4 AiQuadToXMFLOAT4(const aiQuaternion& quad){
	return DirectX::XMFLOAT4(quad.x, quad.y, quad.z, quad.w);
}

bool readSkeleton(std::unordered_map<std::string, std::pair<int, DirectX::XMMATRIX>> boneInfo, Bone& joint, aiNode* node){

	if (boneInfo.find(node->mName.C_Str()) != boneInfo.end()) { // if node is actually a bone
		joint.name = node->mName.C_Str();
		joint.id = boneInfo[joint.name].first;
		joint.inverseBindPoseMatrix = DirectX::XMMatrixTranspose(boneInfo[joint.name].second);
		 
	
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			Bone child;
			//child.parent = &joint;
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
	Bone& rootJoint, 
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
		#endif 
	}
	#ifdef TRADITIONALSKELETALANIMATION
	//normalize weights to make all weights sum 1
	for (int w = 0; w < vertecies.size(); w++) {
		
		float totalWeight = vertecies[w].boneWeights[0]
			+ vertecies[w].boneWeights[1]
			+ vertecies[w].boneWeights[2]
			+ vertecies[w].boneWeights[3];
	
		if (totalWeight > 0.0f) {
			vertecies[w].boneWeights[0] = vertecies[w].boneWeights[0] / totalWeight;
			vertecies[w].boneWeights[1] = vertecies[w].boneWeights[1] / totalWeight;
			vertecies[w].boneWeights[2] = vertecies[w].boneWeights[2] / totalWeight;
			vertecies[w].boneWeights[3] = vertecies[w].boneWeights[3] / totalWeight;
		}
	}
	#endif 
	readSkeleton(boneInfo, rootJoint, node);
}

void addEmptyAnimationForEmptyJoints(Bone& bone, Animation& animation){

	if(animation.keyFrames.find(bone.name) == animation.keyFrames.end()){
		//if the bone doesn't exist in animation add it
		//DO I need two???
		KeyFrame track;
		track.positions.push_back(DirectX::XMFLOAT3(0,0,0));
		track.rotations.push_back(DirectX::XMFLOAT4(0,0,0,1));
		track.scales.push_back(DirectX::XMFLOAT3(1,1,1));

		track.positions.push_back(DirectX::XMFLOAT3(0,0,0));
		track.rotations.push_back(DirectX::XMFLOAT4(0,0,0,1));
		track.scales.push_back(DirectX::XMFLOAT3(1,1,1));
		track.positionTimestamps.push_back(0);
		track.rotationTimestamps.push_back(0);
		track.scaleTimestamps.push_back(0);

		track.positionTimestamps.push_back(animation.length);
		track.rotationTimestamps.push_back(animation.length);
		track.scaleTimestamps.push_back(animation.length);
		animation.keyFrames.insert(std::pair<std::string, KeyFrame>(bone.name, track));
	}
	else{
		//if the bone doesn't have a certain animation
		KeyFrame& track = animation.keyFrames.find(bone.name)->second;
		if(track.positions.size() <= 0){
			track.positions.push_back(DirectX::XMFLOAT3(0, 0, 0));
			track.positionTimestamps.push_back(0);
			track.positions.push_back(DirectX::XMFLOAT3(0, 0, 0));
			track.positionTimestamps.push_back(animation.length);
		}
		if(track.rotations.size() <= 0){
			track.rotations.push_back(DirectX::XMFLOAT4(0, 0, 0, 1));
			track.rotationTimestamps.push_back(0);
			track.rotations.push_back(DirectX::XMFLOAT4(0, 0, 0, 1));
			track.rotationTimestamps.push_back(animation.length);
		}
		if(track.scales.size() <= 0){
			track.scales.push_back(DirectX::XMFLOAT3(1,1,1));
			track.scaleTimestamps.push_back(0);
			track.scales.push_back(DirectX::XMFLOAT3(1,1,1));
			track.scaleTimestamps.push_back(animation.length);
		}
	}
	for(int i = 0; i < bone.childJoints.size(); i++){
		addEmptyAnimationForEmptyJoints(bone.childJoints[i], animation);
	}

}

bool loadAnimation(const aiScene* scene, Animation& animation){
	aiAnimation* anim = scene->mAnimations[0];

	if(anim->mTicksPerSecond != 0.0f){
		animation.tick = (float)anim->mTicksPerSecond;
	}
	else{
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
		animation.keyFrames.insert(std::pair<std::string, KeyFrame>(channel->mNodeName.C_Str(), track));
	}
	return true;
}

void loadParticleModel(std::vector<VolumetricVertex>& vertecies, const std::string& filePath, Animation& animation, Bone& rootJoint)
{
	Assimp::Importer AImporter;
	const aiScene* scene = AImporter.ReadFile(filePath, aiProcess_JoinIdenticalVertices);

	//exit if no scene/file was found
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		std::cout << "ERROR could find scene or file" << std::endl;
		exit(-2);
	}

	aiMesh* mesh = scene->mMeshes[0];
	

	//load mesh/particle form
	for(unsigned int v = 0; v < mesh->mNumVertices; v++){
		aiVector3D vertex = mesh->mVertices[v];
		vertecies.push_back(VolumetricVertex(vertex.x, vertex.y, vertex.z, 0, 0, 1, 0.7f));
	}
	//for(unsigned int v = 0; v < mesh->mNumVertices; v++){
	//	vertecies.push_back(VolumetricVertex(0, 0, v * 0.2f, 0, 0, 1, 0.7f));
	//}
	
	//load Bones
	loadBoneDataToVertecies(vertecies, rootJoint, mesh, scene->mRootNode, (int)vertecies.size());
	
	//load Animation
	loadAnimation(scene, animation);
	addEmptyAnimationForEmptyJoints(rootJoint, animation);
	
}

void getOrginalPositions(
	Bone& Joint,
	std::vector<DirectX::XMMATRIX>& transform)
{
	transform.push_back(DirectX::XMMatrixTranspose(Joint.inverseBindPoseMatrix));


	for(int i = 0; i < Joint.childJoints.size(); i++){
		getOrginalPositions(
			Joint.childJoints[i], 
			transform);
	}
}

void getHitBoxPosition(
	Bone& rootJoint, 
	std::vector<DirectX::XMMATRIX>& transform)
{	
	getOrginalPositions(rootJoint, transform);

}
