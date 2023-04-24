#include "OBBSkeleton.h"
#include <iostream>
#include "CreateBuffer.h"
#include "Random.h"

OBBSkeletonDebug::OBBSkeletonDebug(unsigned int nrOfBones, std::vector<DirectX::XMFLOAT3> & sizes, Graphics*& gfx)
{
	constBufferConverterTime.dt.element = 0;
	if(nrOfBones != sizes.size()){
		std::cout << "not the same size" << std::endl;
	}
	constBufferConverter.nrOfBones.element = nrOfBones;
	for(unsigned int i = 0; i < nrOfBones; i++){
		size.push_back(DirectX::XMMATRIX(
			sizes[i].y, 0, 0, 0,
			0, sizes[i].x, 0, 0,
			0, 0, sizes[i].z, 0,
			0, 0, 0, 1
		));
	}
	constBufferConverterDebugDraw.projection.element = gfx->getVertexconstbuffer()->projection.element;
	constBufferConverterDebugDraw.view.element = gfx->getVertexconstbuffer()->view.element;

	this->constBufferConverterPrev = this->constBufferConverter;

	verteciesPoints.push_back(point(vec3(0.5f, 1.f, 0.5f)));
	verteciesPoints.push_back(point(vec3(0.5f, 1.f, -0.5f)));
	verteciesPoints.push_back(point(vec3(0.5f, 0.f, 0.5f)));
	verteciesPoints.push_back(point(vec3(0.5f, 0.f, -0.5f)));
	verteciesPoints.push_back(point(vec3(-0.5f, 1.f, 0.5f)));
	verteciesPoints.push_back(point(vec3(-0.5f, 1.f, -0.5f)));
	verteciesPoints.push_back(point(vec3(-0.5f, 0.f, 0.5f)));
	verteciesPoints.push_back(point(vec3(-0.5f, 0.f, -0.5f)));

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
	CreateConstBuffer(gfx, constantBuffer, sizeof(OBBSkeletonOBBBuffer), &constBufferConverterPrev);
	CreateConstBuffer(gfx, constantBufferTime, sizeof(OBBSkeletonOBBBufferTime), &constBufferConverterTime);
	CreateConstBuffer(gfx, constantBufferDebugDraw, sizeof(OBBSkeletonOBBBufferDebugDraw), &constBufferConverterDebugDraw);
}

OBBSkeletonDebug::~OBBSkeletonDebug()
{
	vertexBuffer->Release();
	indeciesBuffer->Release();
	constantBuffer->Release();
	constantBufferTime->Release();
	
}

void OBBSkeletonDebug::setTransformations(std::vector<DirectX::XMMATRIX>& transform)
{
	this->transform = transform;
}

void OBBSkeletonDebug::setTransform(int id, const DirectX::XMMATRIX transform)
{
	this->transform[id] = transform;
}

std::vector<DirectX::XMMATRIX>& OBBSkeletonDebug::getTransforms()
{
	return this->transform;
}

void OBBSkeletonDebug::updateObbPosition(Bone& rootjoint, const SkeletonConstantBuffer skeltonConstBuffer)
{

	DirectX::XMMATRIX BoneOrginalPosition = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixTranspose(rootjoint.inverseBindPoseMatrix));

	DirectX::XMMATRIX jointMatrix = BoneOrginalPosition * DirectX::XMMatrixTranspose(skeltonConstBuffer.Transformations.element[rootjoint.id]);

	transform[rootjoint.id] = jointMatrix;

	for(int i = 0; i < rootjoint.childJoints.size(); i++){
		updateObbPosition(rootjoint.childJoints[i], skeltonConstBuffer);
	}
}

void OBBSkeletonDebug::draw(Graphics*& gfx)
{
	UINT offset = 0;
	static UINT strid = sizeof(point);
	//set shaders
	gfx->get_IMctx()->VSSetShader(gfx->getVS()[2], nullptr, 0);
	gfx->get_IMctx()->PSSetShader(gfx->getPS()[3], nullptr, 0);
	gfx->get_IMctx()->GSSetShader(nullptr, nullptr, 0);
	gfx->get_IMctx()->HSSetShader(nullptr, nullptr, 0);
	gfx->get_IMctx()->DSSetShader(nullptr, nullptr, 0);
	
	//remove depthstencil so OBB shows through everything
	//gfx->get_IMctx()->OMSetRenderTargets(1, &gfx->getRenderTarget(), nullptr);
	
	gfx->get_IMctx()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gfx->get_IMctx()->IASetInputLayout(gfx->getInputLayout()[1]);
	gfx->get_IMctx()->VSSetConstantBuffers(0, 1, &constantBufferDebugDraw);
	gfx->get_IMctx()->IASetVertexBuffers(0, 1, &this->vertexBuffer, &strid, &offset);
	gfx->get_IMctx()->IASetIndexBuffer(indeciesBuffer, DXGI_FORMAT_R32_UINT, offset);
	//draw
	gfx->get_IMctx()->DrawIndexedInstanced((UINT)indecies.size(), (UINT)size.size(), 0, 0, 0);
	
	//add depth stencil again
	//gfx->get_IMctx()->OMSetRenderTargets(1, &gfx->getRenderTarget(), gfx->getDepthStencil());


}

ID3D11Buffer*& OBBSkeletonDebug::getSkeletalTransformConstBuffer()
{
	return constantBuffer;
}

ID3D11Buffer*& OBBSkeletonDebug::getSkeletalTimeConstBuffer()
{
	return constantBufferTime;
}

void OBBSkeletonDebug::inverseTransforms()
{
	for(unsigned int i = 0; i < constBufferConverter.nrOfBones.element; i++){
		constBufferConverterPrev.InverseTransform.element[i] = DirectX::XMMatrixInverse(nullptr, constBufferConverterPrev.transform.element[i]);
	}
}

void OBBSkeletonDebug::inverseDeltaTransforms()
{
	//this is not needed
	for(unsigned int i = 0; i < constBufferConverter.nrOfBones.element; i++){
		constBufferConverterPrev.deltaTransform.element[i] = DirectX::XMMatrixInverse(nullptr, constBufferConverterPrev.deltaTransform.element[i]);
	}
}


void OBBSkeletonDebug::update(Graphics*& gfx, float dt)
{
	constBufferConverterTime.dt.element = dt;
	int randNr = RandomNumber(0, int(transform.size()));
	constBufferConverterTime.random.element = randNr;

	this->constBufferConverterPrev = this->constBufferConverter;
	
	//update constantBuffer
	for(int i = 0; i < transform.size(); i++){
		constBufferConverter.transform.element[i] = size[i] * transform[i];
		constBufferConverterDebugDraw.transform.element[i] = size[i] * transform[i];
	}

	constBufferConverterDebugDraw.projection.element = gfx->getVertexconstbuffer()->projection.element;
	constBufferConverterDebugDraw.view.element = gfx->getVertexconstbuffer()->view.element;

	inverseTransforms();

	OBBSkeletonOBBBuffer constBufferConverterDelta = this->constBufferConverter - this->constBufferConverterPrev;

	//plan is to send all delta matrices into shader
	//only delta transform matrix is needed so the delta buffer can be omitted 
	//save deltaElements and send them into some kind of buffer, either modify OBBSkeletonOBBBuffer or and dedicated buffer

	//since the constBufferConverterPrev now is sent to shader, we use it's delta slot

	std::copy(std::begin(constBufferConverterDelta.transform.element), std::end(constBufferConverterDelta.transform.element), constBufferConverterPrev.deltaTransform.element);

	D3D11_MAPPED_SUBRESOURCE resource;
    gfx->get_IMctx()->Map(constantBufferTime, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    memcpy(resource.pData, &constBufferConverterTime, sizeof(OBBSkeletonOBBBufferTime));
    gfx->get_IMctx()->Unmap(constantBufferTime, 0);
    ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	gfx->get_IMctx()->Map(constantBufferDebugDraw, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    memcpy(resource.pData, &constBufferConverterDebugDraw, sizeof(OBBSkeletonOBBBufferDebugDraw));
    gfx->get_IMctx()->Unmap(constantBufferDebugDraw, 0);
    ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));

    gfx->get_IMctx()->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    memcpy(resource.pData, &constBufferConverterPrev, sizeof(OBBSkeletonOBBBuffer));
    gfx->get_IMctx()->Unmap(constantBuffer, 0);
    ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
}
