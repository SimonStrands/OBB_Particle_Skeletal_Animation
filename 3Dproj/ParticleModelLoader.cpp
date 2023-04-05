#include "ParticleModelLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>

void readBoneHiarchy(aiNode *pNode, const aiMatrix4x4& parentMatrix, std::vector<DirectX::XMMATRIX> &Transformations)
{
	aiMatrix4x4 newParentMatrix = parentMatrix * pNode->mTransformation;
	aiMatrix4x4 m = newParentMatrix;
				Transformations.push_back(DirectX::XMMATRIX(
					m.a1, m.b1, m.c1, m.d1,
					m.a2, m.b2, m.c2, m.d2,
					m.a3, m.b3, m.c3, m.d3,
					m.a4 * 0.01f, m.b4 * 0.01f, m.c4 * 0.01f, m.d4
				));
	for(int i = 0; i < pNode->mNumChildren; i++){
		readBoneHiarchy(pNode->mChildren[i], newParentMatrix, Transformations);
	}
}

void loadParticleModel(std::vector<VolumetricVertex>& vertecies, std::vector<DirectX::XMMATRIX> &Transformations, const std::string& filePath)
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
			//std::map<
			
			readBoneHiarchy(scene->mRootNode,aiMatrix4x4(
				1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1), 
				Transformations);
			for(unsigned int b = 0; b < scene->mMeshes[i]->mNumBones; b++){
				
			}
		}
		for(unsigned int v = 0; v < scene->mMeshes[i]->mNumVertices; v++){
			aiVector3D vertex = scene->mMeshes[i]->mVertices[v];
			vertecies.push_back(VolumetricVertex(vertex.x, vertex.y, vertex.z, 0, 0, 1, 0.75f));
		}
	}
}