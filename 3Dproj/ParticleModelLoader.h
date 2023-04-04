#pragma once
#include "vertex.h"
#include <vector>

void loadParticleModel(std::vector<VolumetricVertex>& vertecies, std::vector<DirectX::XMMATRIX> &Transformations, const std::string& filePath);