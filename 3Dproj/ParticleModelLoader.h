#pragma once
#include "vertex.h"
#include <vector>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "SkeletalAnim.h"
#include "Animation.h"
#include "Graphics.h"

static const int maxNumberOfBones = 55;
struct SkeletonConstantBuffer : public CB {
	struct{
		DirectX::XMMATRIX element[maxNumberOfBones];
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
