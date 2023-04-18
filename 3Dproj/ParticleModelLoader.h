#pragma once
#include "vertex.h"
#include <vector>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "SkeletalAnim.h"
#include "Animation.h"
#include "Graphics.h"

static const int maxNumberOfBones = 70;
struct SkeletonConstantBuffer : CB{
	struct{
		DirectX::XMMATRIX element[maxNumberOfBones]; //max number of bones are 70 (NOT FINAL!)
	}Transformations;
};

DirectX::XMMATRIX AiMatrixToXMMATRIX(aiMatrix4x4 mat);

void loadParticleModel(
	std::vector<VolumetricVertex>& vertecies, 
	const std::string& filePath,
	Animation& animation,
	Bone& rootJoint
);

void getHitBoxPosition(
	Bone& rootJoint, 
	std::vector<DirectX::XMMATRIX>& transform);