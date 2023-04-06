#pragma once
#include "vertex.h"
#include <vector>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "SkeletalAnim.h"
#include "Animation.h"

DirectX::XMMATRIX AiMatrixToXMMATRIX(aiMatrix4x4 mat);
void Nodes(int& nrTotal, std::vector<DirectX::XMMATRIX>& arr, aiNode* walker);

void loadParticleModel(std::vector<VolumetricVertex>& vertecies, const std::string& filePath);