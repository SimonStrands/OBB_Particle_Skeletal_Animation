#pragma once
#include "vertex.h"
#include <vector>
//#include <map>
#include <string>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "SkeletalAnim.h"

DirectX::XMFLOAT3 AiVector3ToXMFLOAT3(const aiVector3D& vec3);
DirectX::XMFLOAT4 AiQuadToXMFLOAT4(const aiQuaternion& quad);

DirectX::XMMATRIX AiMatrixToXMMATRIX(aiMatrix4x4 mat);
void Nodes(int& nrTotal, Bone & parent, aiNode* walker);

void testReadHiaechy(aiNode *pNode, const aiMatrix4x4& parentMatrix, std::vector<DirectX::XMMATRIX> &Transformations);

void loadParticleModel(
	std::vector<VolumetricVertex>& vertecies, 
	const std::string& filePath,
	Animation& animation,

	Bone& rootJoint
);

