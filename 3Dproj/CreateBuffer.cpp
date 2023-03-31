#include "CreateBuffer.h"
#include "GameObject.h"
#include "Graphics.h"
/*
bool CreateVertexBuffer(Graphics*& gfx, MeshObj& mesh, std::string fileName) {
	std::vector<std::vector<vertex>> vertices;
	if (fileName.substr(fileName.size() - 3) == "fbx") {
		//readFBXFile(vertices, fileName, mesh.getNrOfVertex());
	}
	else
	{
		if (!readObjFile(vertices, fileName, mesh.getNrOfVertex())) {
			return false;
		}
	}
	

	D3D11_BUFFER_DESC bDesc = {};
	bDesc.ByteWidth = sizeof(vertex) * (UINT)mesh.getNrOfVertex();
	bDesc.Usage = D3D11_USAGE_DYNAMIC;
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertices[0].data();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = gfx->getDevice()->CreateBuffer(&bDesc, &data, &mesh.getVertexBuffer());

	if (FAILED(hr)) {
		printf("failed");
		return false;
	}

	return !FAILED(hr);

}
*/
bool CreateVertexConstBuffer(Graphics*& gfx, ID3D11Buffer*& buff)
{
	D3D11_BUFFER_DESC CbDesc;
	CbDesc.ByteWidth = sizeof(Vcb);
	CbDesc.Usage = D3D11_USAGE_DYNAMIC;
	CbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CbDesc.MiscFlags = 0;
	CbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	InitData.pSysMem = gfx->getVertexconstbuffer();

	HRESULT hr = gfx->getDevice()->CreateBuffer(&CbDesc, &InitData, &buff);
	if (FAILED(hr)) {
		printf("failed");
		return false;
	}

	return !FAILED(hr);
}

bool CreateConstBuffer(Graphics*& gfx, ID3D11Buffer*& buff, UINT size, CB *initdata) {
	D3D11_BUFFER_DESC CbDesc;
	CbDesc.ByteWidth = size;
	CbDesc.Usage = D3D11_USAGE_DYNAMIC;
	CbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CbDesc.MiscFlags = 0;
	CbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;

	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	InitData.pSysMem = &initdata;

	HRESULT hr = gfx->getDevice()->CreateBuffer(&CbDesc, &InitData, &buff);
	if (FAILED(hr)) {
		printf("failed");
		return false;
	}

	return !FAILED(hr);
}

bool CreatePixelConstBuffer(Graphics*& gfx, ID3D11Buffer*& buff)
{

	D3D11_BUFFER_DESC CbDesc;
	CbDesc.ByteWidth = sizeof(Pcb);
	CbDesc.Usage = D3D11_USAGE_DYNAMIC;
	CbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CbDesc.MiscFlags = 0;
	CbDesc.StructureByteStride = 0;
	
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = gfx->getPixelconstbuffer();
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	HRESULT hr = gfx->getDevice()->CreateBuffer(&CbDesc, &InitData, &buff);

	return FAILED(hr);
}

bool CreateGeometryConstBuffer(Graphics*& gfx, ID3D11Buffer*& buff)
{
	D3D11_BUFFER_DESC CbDesc;
	CbDesc.ByteWidth = sizeof(Gcb);
	CbDesc.Usage = D3D11_USAGE_DYNAMIC;
	CbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CbDesc.MiscFlags = 0;
	CbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = gfx->getPixelconstbuffer();
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	HRESULT hr = gfx->getDevice()->CreateBuffer(&CbDesc, &InitData, &buff);

	return FAILED(hr);
}

