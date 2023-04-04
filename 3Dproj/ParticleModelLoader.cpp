#include "ParticleModelLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
			//for(unsigned int b = 0; b < scene->mMeshes[i]->mNumBones; i++){
			//	scene->mMeshes[i]->mBones[b]->mName;
			//	scene->mMeshes[i]->mBones[b]->mWeights;
			//	scene->mMeshes[i]->mBones[b]->mOffsetMatrix;
			//	scene->mMeshes[i]->
			//}
		}
		for(unsigned int v = 0; v < scene->mMeshes[i]->mNumVertices; v++){
			aiVector3D vertex = scene->mMeshes[i]->mVertices[v];
			vertecies.push_back(VolumetricVertex(vertex.x, vertex.y, vertex.z, 0, 0, 1, 0.75f));
		}
	}
}
