#include "ParticleModelLoader.h"



DirectX::XMMATRIX AiMatrixToXMMATRIX(aiMatrix4x4 mat)
{
	DirectX::XMFLOAT4X4 mat4;
	mat4._11 = mat.a1;
	mat4._12 = mat.a2;
	mat4._13 = mat.a3;
	mat4._14 = mat.a4;
	mat4._21 = mat.b1;
	mat4._22 = mat.b2;
	mat4._23 = mat.b3;
	mat4._24 = mat.b4;
	mat4._31 = mat.c1;
	mat4._32 = mat.c2;
	mat4._33 = mat.c3;
	mat4._34 = mat.c4;
	mat4._41 = mat.d1;
	mat4._42 = mat.d2;
	mat4._43 = mat.d3;
	mat4._44 = mat.d4;
	DirectX::XMMATRIX xmat = DirectX::XMLoadFloat4x4(&mat4);
	return xmat;
}
void Nodes(int & nrTotal, std::vector<DirectX::XMMATRIX> &arr, aiNode* walker)
{
	arr.push_back(AiMatrixToXMMATRIX(walker->mTransformation));

	for (int i = 0; i < walker->mNumChildren; i++)
	{
		aiNode* temp = walker->mChildren[i];
		Nodes(nrTotal, arr, temp);
	}
	
	
}
void loadParticleModel(std::vector<VolumetricVertex>& vertecies, const std::string& filePath)
{
	Assimp::Importer AImporter;
	const aiScene* scene = AImporter.ReadFile(filePath, aiProcess_JoinIdenticalVertices);

	for(unsigned int i = 0; i < scene->mNumMeshes; i++){
		if(scene->mMeshes[i]->HasBones()){

			//load animations and bones
			//need:
			//name
			//bone transformation
			//inverse?
			//scene->mMeshes[i].
			//scene->mMeshes[i].
			
			//if (scene->HasAnimations())
			//	std::cout << "test";
			//scene->mAnimations
			std::string name = scene->mRootNode->mName.C_Str();
			DirectX::XMMATRIX mat = AiMatrixToXMMATRIX(scene->mRootNode->mTransformation); //first keyframe
			
			
			//scene->mR
			std::vector<DirectX::XMMATRIX> arr;
			aiNode* walker = scene->mRootNode; // ->mChildren[0]->mTransformation
			int nrC = scene->mRootNode->mNumChildren;
			
			//int totalNr

			//for(int i=0;i<)
			Joint(0,name, mat);

			//parant matrix mul current

			for(unsigned int b = 0; b < scene->mMeshes[i]->mNumBones; i++){
				

				std::string name = scene->mMeshes[i]->mBones[b]->mName.C_Str();
				//scene->mMeshes[i]->mBones[b]->mWeights;

			
				DirectX::XMMATRIX xmat = AiMatrixToXMMATRIX(scene->mMeshes[i]->mBones[b]->mOffsetMatrix);

				
				//scene->mMeshes[i]->mBones[b]->mNumWeights;
				Joint temp(b,name,xmat);
			}
		}
		for(unsigned int v = 0; v < scene->mMeshes[i]->mNumVertices; v++){
			aiVector3D vertex = scene->mMeshes[i]->mVertices[v];
			vertecies.push_back(VolumetricVertex(vertex.x, vertex.y, vertex.z, 0, 0, 1, 0.75f));
		}
	}
}
