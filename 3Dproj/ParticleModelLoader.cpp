#include "ParticleModelLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <assimp/Importer.hpp>

void loadParticleModel(std::vector<VolumetricVertex>& vertecies, const std::string& filePath)
{
	std::ifstream infile(filePath);
	float x,y,z;
	std::string trash;
	std::string readWord;
	bool useOfG = false;
	bool first = true;
	if (!infile.is_open()) {
		std::cout << "cannot find file" << std::endl;
		return;
	}

	int c = 0;
	while (std::getline(infile, readWord)) {
		if (readWord.substr(0, 2) == "v ") {
			std::istringstream a;
			a.str(readWord);
			a >> trash >> x >> y >> z;
			vertecies.push_back(VolumetricVertex(x, y, z, 0, 0, 255, 0.75f));
			c++;
		}
	}
}
