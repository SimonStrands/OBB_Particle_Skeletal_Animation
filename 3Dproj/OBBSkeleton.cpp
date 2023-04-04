#include "OBBSkeleton.h"
#include <iostream>
#include "CreateBuffer.h"

OBBSkeletonDebug::OBBSkeletonDebug(std::vector<DirectX::XMMATRIX>& transform, std::vector<float>& height, Graphics*& gfx)
{
	if(transform.size() != height.size()){
		std::cout << "not the same size" << std::endl;
	}
	this->transform = transform;
	for(int i = 0; i < height.size(); i++){
		size.push_back(DirectX::XMMATRIX(
			OBBWidth, 0, 0, 0,
			0, height[i], 0, 0,
			0, 0, OBBDepth, 0,
			0, 0, 0, 1
		));
	}
	for(int i = 0; i < transform.size(); i++){
		constBufferConverter.transform.element[i] = size[i] * transform[i];
	}
	constBufferConverter.projection.element = gfx->getVertexconstbuffer()->projection.element;
	constBufferConverter.view.element = gfx->getVertexconstbuffer()->view.element;

	verteciesPoints.push_back(point(vec3(0.5f, 0.5f, 0.5f)));
	verteciesPoints.push_back(point(vec3(0.5f, 0.5f, -0.5f)));
	verteciesPoints.push_back(point(vec3(0.5f, -0.5f, 0.5f)));
	verteciesPoints.push_back(point(vec3(0.5f, -0.5f, -0.5f)));
	verteciesPoints.push_back(point(vec3(-0.5f, 0.5f, 0.5f)));
	verteciesPoints.push_back(point(vec3(-0.5f, 0.5f, -0.5f)));
	verteciesPoints.push_back(point(vec3(-0.5f, -0.5f, 0.5f)));
	verteciesPoints.push_back(point(vec3(-0.5f, -0.5f, -0.5f)));

	indecies = {
        0,2,1,
	    3,1,2,
	    4,5,6,
	    5,7,6,
		5,1,7,
		1,3,7,
		0,4,6,
		0,6,2,
		4,1,5,
		4,0,1,
		3,2,7,
		6,7,2
    };
	CreateVertexBuffer(gfx->getDevice(), verteciesPoints, vertexBuffer, false);
	CreateVertexBuffer(gfx->getDevice(), indecies, indeciesBuffer, true);
	CreateConstBuffer(gfx, constantBuffer, sizeof(OBBSkeletonOBBBuffer), &constBufferConverter);
}

OBBSkeletonDebug::~OBBSkeletonDebug()
{
	vertexBuffer->Release();
	indeciesBuffer->Release();
	constantBuffer->Release();
}

void OBBSkeletonDebug::setTransformations(std::vector<DirectX::XMMATRIX>& transform)
{
	this->transform = transform;
}

void OBBSkeletonDebug::draw(Graphics*& gfx)
{
	UINT offset = 0;
	static UINT strid = sizeof(point);
	//set shaders
	update(gfx);
	gfx->get_IMctx()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gfx->get_IMctx()->IASetInputLayout(gfx->getInputLayout()[1]);
	gfx->get_IMctx()->VSSetConstantBuffers(0, 1, &constantBuffer);
	gfx->get_IMctx()->IASetVertexBuffers(0, 1, &this->vertexBuffer, &strid, &offset);
	gfx->get_IMctx()->IASetIndexBuffer(indeciesBuffer, DXGI_FORMAT_R32_UINT, offset);
	//draw
	//gfx->get_IMctx()->Draw(verteciesPoints.size(), 0);
	//gfx->get_IMctx()->DrawIndexed(indecies.size(), 0, 0);
	gfx->get_IMctx()->DrawIndexedInstanced(indecies.size(), size.size(), 0, 0, 0);

}

void OBBSkeletonDebug::update(Graphics*& gfx)
{
	//set shaders
	gfx->get_IMctx()->VSSetShader(gfx->getVS()[2], nullptr, 0);
	gfx->get_IMctx()->PSSetShader(gfx->getPS()[3], nullptr, 0);
	gfx->get_IMctx()->GSSetShader(nullptr, nullptr, 0);
	gfx->get_IMctx()->HSSetShader(nullptr, nullptr, 0);
	gfx->get_IMctx()->DSSetShader(nullptr, nullptr, 0);

	//update constantBuffer
	for(int i = 0; i < transform.size(); i++){
		constBufferConverter.transform.element[i] = size[i] * transform[i];
	}
	constBufferConverter.projection.element = gfx->getVertexconstbuffer()->projection.element;
	constBufferConverter.view.element = gfx->getVertexconstbuffer()->view.element;
	
	D3D11_MAPPED_SUBRESOURCE resource;
    gfx->get_IMctx()->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    memcpy(resource.pData, &constBufferConverter, sizeof(OBBSkeletonOBBBuffer));
    gfx->get_IMctx()->Unmap(constantBuffer, 0);
    ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
}
