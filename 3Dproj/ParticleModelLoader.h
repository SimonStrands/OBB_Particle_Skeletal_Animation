#pragma once
#include "vertex.h"
#include <vector>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "SkeletalAnim.h"

DirectX::XMMATRIX AiMatrixToXMMATRIX(aiMatrix4x4 mat);
void Nodes(int& nrTotal,Joint * parent, aiNode* walker);

void loadParticleModel(std::vector<VolumetricVertex>& vertecies, const std::string& filePath);
void loadSkeletalModel(Joint*& root, const std::string& filePath);