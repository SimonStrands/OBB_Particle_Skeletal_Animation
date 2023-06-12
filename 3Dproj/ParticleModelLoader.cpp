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
	int verteciesSize,
	std::map<int, IdAndWeight>& idandWeight
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
#ifndef ONEBONE
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
#endif // ONEBONE
			default:

				break;

			}
		}
        #else
		for(int i = 0; i < verteciesSize; i++){
			idandWeight.insert(std::pair<int, IdAndWeight>(i, IdAndWeight()));
		}
		for (unsigned int j = 0; j < bone->mNumWeights; j++) {
			unsigned int id = bone->mWeights[j].mVertexId;
			float weight = bone->mWeights[j].mWeight;
			boneCounts[id]++;
			switch (boneCounts[id]) {
			case 1:
				idandWeight[id].IDs.xyz.x = (float)i;
				idandWeight[id].weights.xyz.x = weight;
				break;
			case 2:
				idandWeight[id].IDs.xyz.y = (float)i;
				idandWeight[id].weights.xyz.y = weight;
				break;
			case 3:
				idandWeight[id].IDs.xyz.z = (float)i;
				idandWeight[id].weights.xyz.z = weight;
				break;
			case 4:
				idandWeight[id].IDs.w = (float)i;
				idandWeight[id].weights.w = weight;
				break;
			default:
		
				break;
		
			}
		}
		#endif

	}
	#ifdef TRADITIONALSKELETALANIMATION
#ifdef ONEBONE
	//normalize weights to make all weights sum 1
	for (int w = 0; w < vertecies.size(); w++) {
		vertecies[w].boneWeights[0] = 1;
	}
#else
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
#endif // ONEBONE

	
    #else
	for (auto & x : idandWeight){
		
		float totalWeight = x.second.weights.xyz.x
			+ x.second.weights.xyz.y
			+ x.second.weights.xyz.z
			+ x.second.weights.w;
	
		if (totalWeight > 0.0f) {
			x.second.weights.xyz.x = x.second.weights.xyz.x / totalWeight;
			x.second.weights.xyz.y = x.second.weights.xyz.y / totalWeight;
			x.second.weights.xyz.z = x.second.weights.xyz.z / totalWeight;
			x.second.weights.w     = x.second.weights.w     / totalWeight;
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

float vertexLenght(const VolumetricVertex& a, const VolumetricVertex& b){
	return sqrt(
		a.pos[0] - b.pos[0] * a.pos[0] - b.pos[0] + 
		a.pos[1] - b.pos[1] * a.pos[1] - b.pos[1] + 
		a.pos[2] - b.pos[2] * a.pos[2] - b.pos[2]
	);
}

vec3 lerp(const VolumetricVertex& a, const VolumetricVertex& b, float procent){
	vec3 theReturn;
	theReturn.x = a.pos[0] + ((b.pos[0] - a.pos[0]) * procent);
	theReturn.y = a.pos[1] + ((b.pos[1] - a.pos[1]) * procent);
	theReturn.z = a.pos[2] + ((b.pos[2] - a.pos[2]) * procent);
	return theReturn;
}

vec3 lerp(const vec3& a, const vec3& b, float procent){
	vec3 theReturn;
	theReturn.x = a.x + ((b.x - a.x) * procent);
	theReturn.y = a.y + ((b.y - a.y) * procent);
	theReturn.z = a.z + ((b.z - a.z) * procent);
	return theReturn;
}

vec3 setColor(const std::vector<vec3>& c, const vec3& v){
	vec3 theReturn;
	theReturn = c[RandomNumber(0, (int)c.size())];
	return theReturn;
}

void subDivide(
	int subDivision, 
	VolumetricVertex newVertecies[],
	unsigned int id[],
	std::vector<VolumetricVertex> &vertecies, 
	const std::vector<vec3>& colors,
	std::map<int, IdAndWeight>& idandWeight
)
{
	if(subDivision < 1){
		return;
	}
	subDivision = subDivision - 1;
	vec3 c;
	vec3 vx = lerp(newVertecies[0], newVertecies[1], 0.5f);
	c = setColor(colors, vx);
	VolumetricVertex x(vx.x, vx.y, vx.z, c.x, c.y, c.z, 1.0f);

	vec3 vy = lerp(newVertecies[1], newVertecies[2], 0.5f);
	c = setColor(colors, vy);
	VolumetricVertex y(vy.x, vy.y, vy.z, c.x, c.y, c.z, 1.0f);

	vec3 vz = lerp(newVertecies[2], newVertecies[0], 0.5f);
	c = setColor(colors, vz);
	VolumetricVertex z(vz.x, vz.y, vz.z, c.x, c.y, c.z, 1.0f);

	//find the middle of the triangle
	vec3 vm = vec3((vx.x + vy.x + vz.x) / 3, (vx.y + vy.y + vz.y) / 3, (vx.z + vy.z + vz.z) / 3);
	c = setColor(colors, vm);
	VolumetricVertex m(vm.x, vm.y, vm.z, c.x, c.y, c.z, 1.0f);

#ifdef TRADITIONALSKELETALANIMATION
	x.boneIDs[0]		= newVertecies[0].boneIDs[0];
	x.boneWeights[0]    = newVertecies[0].boneWeights[0];
#ifndef ONEBONE

	x.boneIDs[1]		= newVertecies[0].boneIDs[1];
	x.boneIDs[2]		= newVertecies[0].boneIDs[2];
	x.boneIDs[3]		= newVertecies[0].boneIDs[3];

	x.boneWeights[1]    = newVertecies[0].boneWeights[1];
	x.boneWeights[2]    = newVertecies[0].boneWeights[2];
	x.boneWeights[3]    = newVertecies[0].boneWeights[3];
#endif // !ONEBONE
	
	y.boneIDs[0]        = newVertecies[1].boneIDs[0];
	y.boneWeights[0]    = newVertecies[1].boneWeights[0];
#ifndef ONEBONE
	y.boneIDs[1]        = newVertecies[1].boneIDs[1];
	y.boneIDs[2]        = newVertecies[1].boneIDs[2];
	y.boneIDs[3]        = newVertecies[1].boneIDs[3];
	y.boneWeights[1]    = newVertecies[1].boneWeights[1];
	y.boneWeights[2]    = newVertecies[1].boneWeights[2];
	y.boneWeights[3]    = newVertecies[1].boneWeights[3];
#endif // !ONEBONE
	
	z.boneIDs[0]        = newVertecies[2].boneIDs[0];
	z.boneWeights[0]    = newVertecies[2].boneWeights[0];
#ifndef ONEBONE
	z.boneIDs[1]        = newVertecies[2].boneIDs[1];
	z.boneIDs[2]        = newVertecies[2].boneIDs[2];
	z.boneIDs[3]        = newVertecies[2].boneIDs[3];
	z.boneWeights[1]    = newVertecies[2].boneWeights[1];
	z.boneWeights[2]    = newVertecies[2].boneWeights[2];
	z.boneWeights[3]    = newVertecies[2].boneWeights[3];
#endif // !ONEBONE
	
	m.boneIDs[0]       = newVertecies[2].boneIDs[0];
	m.boneWeights[0]   = newVertecies[2].boneWeights[0]; 
#ifndef ONEBONE
	m.boneIDs[1]       = newVertecies[2].boneIDs[1];
	m.boneIDs[2]       = newVertecies[2].boneIDs[2];
	m.boneIDs[3]       = newVertecies[2].boneIDs[3];
	m.boneWeights[1]   = newVertecies[2].boneWeights[1]; 
	m.boneWeights[2]   = newVertecies[2].boneWeights[2]; 
	m.boneWeights[3]   = newVertecies[2].boneWeights[3]; 
#endif // !ONEBONE
#else
	idandWeight.insert(std::pair<int, IdAndWeight>((int)vertecies.size(), idandWeight[id[0]]));
	idandWeight.insert(std::pair<int, IdAndWeight>((int)vertecies.size() + 1, idandWeight[id[1]]));
	idandWeight.insert(std::pair<int, IdAndWeight>((int)vertecies.size() + 2, idandWeight[id[2]]));
	idandWeight.insert(std::pair<int, IdAndWeight>((int)vertecies.size() + 3, idandWeight[id[2]]));
#endif // TRADITIONALSKELETALANIMATION


	//add the points to the list
	vertecies.push_back(x);
	vertecies.push_back(y);
	vertecies.push_back(z);
	vertecies.push_back(m);

	if(subDivision < 1){
		return;
	}

	//subdivide
	// m -> a -> x
	VolumetricVertex tempArray[3] = {m, newVertecies[0], x};
	subDivide(subDivision - 1, tempArray, id, vertecies, colors, idandWeight);
	// m -> x -> b
	tempArray[1] = x;
	tempArray[2] = newVertecies[1];
	subDivide(subDivision, tempArray, id, vertecies, colors, idandWeight);
	// m -> b -> y
	tempArray[1] = newVertecies[1];
	tempArray[2] = y;
	subDivide(subDivision, tempArray, id, vertecies, colors, idandWeight);
	// m -> y -> c
	tempArray[1] = y;
	tempArray[2] = newVertecies[2];
	subDivide(subDivision, tempArray, id, vertecies, colors, idandWeight);
	// m -> c -> z
	tempArray[1] = newVertecies[2];
	tempArray[2] = z;
	subDivide(subDivision, tempArray, id, vertecies, colors, idandWeight);
	// m -> z -> a
	tempArray[1] = z;
	tempArray[2] = newVertecies[0];
	subDivide(subDivision, tempArray, id, vertecies, colors, idandWeight);
}

void loadParticleModel(
	std::vector<VolumetricVertex>& vertecies, 
	const std::string& filePath, 
	Animation& animation, 
	Bone& rootJoint,
	std::map<int, IdAndWeight>& idandWeight,
	Graphics*& gfx
)
{
	Assimp::Importer AImporter;
	const aiScene* scene = AImporter.ReadFile(filePath, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate);

	//exit if no scene/file was found
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
		std::cout << "ERROR could find scene or file" << std::endl;
		exit(-2);
	}
	
	aiMesh* mesh = scene->mMeshes[0];
	
	static const float nl = 1.00;
	std::vector<vec3> colors;

	if(scene->HasMaterials() || scene->HasTextures()){
		aiMaterial* pMaterial = scene->mMaterials[0];
		aiString path;
		pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL);
		std::string p(path.data);
		//getPixelArray("objects/sandTexture.jpg", colors);
		//getPixelArray("objects/waterTex.jpg", colors);
		colors.push_back(vec3(0,0,1));
    }

	//load mesh/particle form
	for(unsigned int v = 0; v < mesh->mNumVertices; v++){
		aiVector3D vertex = mesh->mVertices[v];
		vec3 c = setColor(colors, vec3(vertex.x, vertex.y, vertex.z));
		vertecies.push_back(VolumetricVertex(vertex.x, vertex.y, vertex.z, c.x, c.y, c.z, 1.0f));
	}



	if(scene->HasAnimations()){
		//load Bones
		loadBoneDataToVertecies(vertecies, rootJoint, mesh, scene->mRootNode, (int)vertecies.size(), idandWeight);
		
		//load Animation
		loadAnimation(scene, animation);
		addEmptyAnimationForEmptyJoints(rootJoint, animation);
	}
	
	
	//add more particle if needed
	for(unsigned int f = 0; f < mesh->mNumFaces; f++){
		const int R = 2;//3;
	
		VolumetricVertex tempArray[3] = {
			vertecies[mesh->mFaces[f].mIndices[0]], 
			vertecies[mesh->mFaces[f].mIndices[1]], 
			vertecies[mesh->mFaces[f].mIndices[2]]
		};
	
		unsigned int verteciesID[] = {mesh->mFaces[f].mIndices[0], mesh->mFaces[f].mIndices[0], mesh->mFaces[f].mIndices[0]};
		subDivide(R, tempArray, verteciesID, vertecies, colors, idandWeight);
	}

	std::cout << "Number of particles " << vertecies.size() << std::endl;
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
