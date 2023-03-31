#include "Material.h"
#include "TrashCollector.h"
#include "ResourceManager.h"

Material::Material()
{
	texSRVPS = new ID3D11ShaderResourceView * [4];
	texSRVDS = new ID3D11ShaderResourceView * [1];
	for (int i = 0; i < 4; i++) {
		texSRVPS[i] = nullptr;//do I need to do this?
	}
	for (int i = 0; i < 1; i++) {
		texSRVDS[i] = nullptr;//do I need to do this?
	}
}

Material::Material(ID3D11ShaderResourceView** def)
{
	texSRVPS = new ID3D11ShaderResourceView * [4];
	texSRVDS = new ID3D11ShaderResourceView * [1];
	for (int i = 0; i < 4; i++) {
		texSRVPS[i] = def[i];//do I need to do this?
	}
	for (int i = 0; i < 1; i++) {
		texSRVDS[i] = nullptr;//do I need to do this?
	}
	for (int i = 0; i < 3; i++) {
		this->Ka[i] = 0.3f;
		this->Kd[i] = 0.8f;
		this->Ks[i] = 0.1f;
	}
	this->Ns = 0;
	this->name = "def";
}

Material::Material(const Material& other)
{
	for (int i = 0; i < 5; i++) {
		this->flags.Maps[i] = other.flags.Maps[i];
	}
	for (int i = 0; i < 3; i++) {
		this->Ka[i] = other.Ka[i];
		this->Kd[i] = other.Kd[i];
		this->Ks[i] = other.Ks[i];
	}
	this->Ns = other.Ns;
	this->name = other.name;
	this->texSRVPS = new ID3D11ShaderResourceView * [4];
	this->texSRVDS = new ID3D11ShaderResourceView * [1];

	for (int i = 0; i < 4; i++) {
		this->texSRVPS[i] = other.texSRVPS[i];
	}
	this->texSRVDS[0] = other.texSRVDS[0];
}

Material& Material::operator=(const Material& other)
{
	for (int i = 0; i < 5; i++) {
		this->flags.Maps[i] = other.flags.Maps[i];
	}
	for (int i = 0; i < 3; i++) {
		this->Ka[i] = other.Ka[i];
		this->Kd[i] = other.Kd[i];
		this->Ks[i] = other.Ks[i];
	}
	this->Ns = other.Ns;
	this->name = other.name;
	this->texSRVPS = new ID3D11ShaderResourceView * [4];
	this->texSRVDS = new ID3D11ShaderResourceView * [1];

	for (int i = 0; i < 4; i++) {
		this->texSRVPS[i] = other.texSRVPS[i];
	}
	this->texSRVDS[0] = other.texSRVDS[0];
	return *this;
}

Material::~Material()
{
	delete[] this->texSRVDS;
	delete[] this->texSRVPS;
}

void Material::loadTexture(std::string filename, Graphics*& gfx, int WhatRSV, ResourceManager* rm)
{
	if (WhatRSV == 4) {
		if (rm->getTexture(filename, gfx, this->texSRVDS[0])) {
			flags.Maps[WhatRSV] = true;
		}
		else {
			this->texSRVDS[0] = nullptr;
		}
	}
	else {
		if (rm->getTexture(filename, gfx, this->texSRVPS[WhatRSV])) {
			flags.Maps[WhatRSV] = true;
		}	
		else {
			this->texSRVPS[WhatRSV] = rm->getDef()[WhatRSV];
		}
	}
}

void Material::begone()
{
	delete[] this->texSRVDS;
	delete[] this->texSRVPS;
}
