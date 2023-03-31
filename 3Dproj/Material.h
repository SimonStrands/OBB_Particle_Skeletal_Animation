#pragma once
#include <d3d11.h>
#include "Graphics.h"

class ResourceManager;

struct textureFlags {
	bool Maps[5] = { false, false, false, false, false };
				// map_Kd, map_Ka, map_Ks, map_Bumb, map_Disp
};

struct Material {
	Material();
	Material(ID3D11ShaderResourceView** def);
	Material(const Material& other);
	Material& operator=(const Material& other);
	~Material();
	void loadTexture(std::string filename, Graphics*& gfx, int WhatRSV, ResourceManager* rm);
	void begone();
	textureFlags flags;
	float Ns = 0;
	float Ka[3] = { 0,0,0 };
	float Kd[3] = { 0,0,0 };
	float Ks[3] = { 0,0,0 };
	std::string name = "";
	ID3D11ShaderResourceView** texSRVPS;
	ID3D11ShaderResourceView** texSRVDS;
};
