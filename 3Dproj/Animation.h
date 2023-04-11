#ifndef ANIMATION_H
#define ANIMATION_H

#include <DirectXMath.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>

struct KeyFrame{
	std::vector<float> positionTimestamps;
	std::vector<float> rotationTimestamps;
	std::vector<float> scaleTimestamps;

	std::vector<DirectX::XMFLOAT3> positions = {};
	std::vector<DirectX::XMFLOAT4> rotations = {};
	std::vector<DirectX::XMFLOAT3> scales = {};
};

class Animation {
public:
	float length;
	float tick = 1.0f;
	std::map<std::string, KeyFrame> keyFrames;
};

#endif // !ANIMATION_H