#include "ParticleModelLoader.h"



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
void Nodes(int & nrTotal, Joint* parent, aiNode* walker)
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

		parent->addChild(&childJoint);
		Joint* nextJoint = parent->GetChildJoints()[i]; //traverse

		Nodes(nrTotal, nextJoint, temp);
	}
	
	
}
void loadParticleModel(std::vector<VolumetricVertex>& vertecies, const std::string& filePath)
{
	Assimp::Importer AImporter;
	const aiScene* scene = AImporter.ReadFile(filePath, aiProcess_JoinIdenticalVertices);


	for(unsigned int i = 0; i < scene->mNumMeshes; i++){
		
		for (unsigned int v = 0; v < scene->mMeshes[i]->mNumVertices; v++) {
			aiVector3D vertex = scene->mMeshes[i]->mVertices[v];
			vertecies.push_back(VolumetricVertex(vertex.x, vertex.y, vertex.z, 0, 0, 1, 0.75f));
		}
		if (scene->HasAnimations())
		{
			for (int a = 0; a < scene->mNumAnimations; a++)
			{
				for (int c = 0; c < scene->mAnimations[a]->mNumChannels;c++)
				{
					int nrofpos = scene->mAnimations[a]->mChannels[c]->mNumPositionKeys;
					int nrofrot = scene->mAnimations[a]->mChannels[c]->mNumRotationKeys;
					
					std::map<std::string, JointTransform> keyframes;
					//scene->mAnimations[a]->mChannels[c]->mNodeName
					//scene->mAnimations[a]->mChannels[c]->
					aiNodeAnim test1;
					aiNode test2;
					for (int p = 0;p < scene->mAnimations[a]->mChannels[c]->mNumPositionKeys; p++)
					{
						DirectX::XMFLOAT3 pos;
						
						pos.x = scene->mAnimations[a]->mChannels[c]->mPositionKeys[p].mValue.x;
						pos.y = scene->mAnimations[a]->mChannels[c]->mPositionKeys[p].mValue.y;
						pos.z = scene->mAnimations[a]->mChannels[c]->mPositionKeys[p].mValue.z;
						float timeP = scene->mAnimations[a]->mChannels[c]->mPositionKeys[p].mTime;
					//}
					//for (int r = 0; r < scene->mAnimations[a]->mChannels[c]->mNumRotationKeys; r++)
					//{
						DirectX::XMVECTOR rot;
						DirectX::XMFLOAT4 rot4;
						rot4.x = scene->mAnimations[a]->mChannels[c]->mRotationKeys[p].mValue.x;
						rot4.y = scene->mAnimations[a]->mChannels[c]->mRotationKeys[p].mValue.y;
						rot4.z = scene->mAnimations[a]->mChannels[c]->mRotationKeys[p].mValue.z;
						rot4.w = scene->mAnimations[a]->mChannels[c]->mRotationKeys[p].mValue.w;
						rot = DirectX::XMLoadFloat4(&rot4);

						float timeR = scene->mAnimations[a]->mChannels[c]->mRotationKeys[p].mTime;
						//what to do with joint transform? save them into keyframes and createa animation object
						JointTransform temp = JointTransform(pos, rot);

					}
					
					

					
				}
				
			}
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
			Joint rootJoint = Joint(totalnr, name, mat);

			Nodes(totalnr, &rootJoint, walker);


			//parant matrix mul current

			for(unsigned int b = 0; b < scene->mMeshes[i]->mNumBones; b++){
				

				std::string name = scene->mMeshes[i]->mBones[b]->mName.C_Str();
				//scene->mMeshes[i]->mBones[b]->mWeights;
				
			
				DirectX::XMMATRIX xmat = AiMatrixToXMMATRIX(scene->mMeshes[i]->mBones[b]->mOffsetMatrix);

				
				//scene->mMeshes[i]->mBones[b]->mNumWeights;
				//Joint temp(b,name,xmat);
			}
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

			Nodes(totalnr,&rootJoint, walker);
		

		}
	}
}
