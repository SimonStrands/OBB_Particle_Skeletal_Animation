/*
 * Copyright (c) 2008-2017, NVIDIA CORPORATION.  All rights reserved.
 *
 * NVIDIA CORPORATION and its licensors retain all intellectual property
 * and proprietary rights in and to this software, related documentation
 * and any modifications thereto.  Any use, reproduction, disclosure or
 * distribution of this software and related documentation without an express
 * license agreement from NVIDIA CORPORATION is strictly prohibited.
 */

#pragma once

#include "../Graphics.h"
#include "renderTargetD3D11.h"

struct FluidShaderConst
{
	DirectX::XMFLOAT4X4 modelViewProjection;
	DirectX::XMFLOAT4X4 modelView;
	DirectX::XMFLOAT4X4 projection;			// ogl projection
	DirectX::XMFLOAT4X4 inverseModelView;
	DirectX::XMFLOAT4X4 inverseProjection;		// ogl inverse projection

	DirectX::XMFLOAT4 invTexScale;

	DirectX::XMFLOAT3 invViewport;
	float _pad0;
	//float3 invProjection;
	//float _pad1;

	float blurRadiusWorld;
	float blurScale;
	float blurFalloff;
	int debug;

	DirectX::XMFLOAT3 lightPos;
	float _pad1;
	DirectX::XMFLOAT3 lightDir;
	float _pad2;
	DirectX::XMFLOAT4X4 lightTransform;

	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT4 clipPosToEye;

	float spotMin;
	float spotMax;
	float ior;
	//float _pad3;
	float pointRadius;  // point size in world space

	DirectX::XMFLOAT4 shadowTaps[12];
};

struct FluidShaderConst
{
	DirectX::XMFLOAT4X4 modelViewProjection;
	DirectX::XMFLOAT4X4 modelView;
	DirectX::XMFLOAT4X4 projection;			// ogl projection
	DirectX::XMFLOAT4X4 inverseModelView;
	DirectX::XMFLOAT4X4 inverseProjection;		// ogl inverse projection

	DirectX::XMFLOAT4 invTexScale;

	DirectX::XMFLOAT3 invViewport;
	float _pad0;
	//float3 invProjection;
	//float _pad1;

	float blurRadiusWorld;
	float blurScale;
	float blurFalloff;
	int debug;

	DirectX::XMFLOAT3 lightPos;
	float _pad1;
	DirectX::XMFLOAT3 lightDir;
	float _pad2;
	DirectX::XMFLOAT4X4 lightTransform;

	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT4 clipPosToEye;

	float spotMin;
	float spotMax;
	float ior;
	//float _pad3;
	float pointRadius;  // point size in world space

	DirectX::XMFLOAT4 shadowTaps[12];
};

struct FluidRenderBuffersD3D11
{
	FluidRenderBuffersD3D11():
		m_positionsBuf(nullptr),
		m_densitiesBuf(nullptr),
		m_indicesBuf(nullptr)
	{
		for (int i = 0; i < 3; i++)
		{
			m_anisotropiesBufArr[i] = nullptr;
		}
		m_numParticles = 0;
	}
	~FluidRenderBuffersD3D11()
	{
	}

	int m_numParticles;
	ID3D11Buffer* m_positions;
	ID3D11Buffer* m_densities;
	ID3D11Buffer* m_anisotropiesArr[3];
	ID3D11Buffer* m_indices;

	ID3D11Buffer* m_fluid; // to be removed

	// wrapper buffers that allow Flex to write directly to VBOs
	ID3D11Buffer* m_positionsBuf;
	ID3D11Buffer* m_densitiesBuf;
	ID3D11Buffer* m_anisotropiesBufArr[3];
	ID3D11Buffer* m_indicesBuf;
};

struct FluidRendererD3D11
{	
	void init(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height);
	
	void drawThickness(const FluidRenderBuffersD3D11* buffers);
	void drawEllipsoids(const FluidRenderBuffersD3D11* buffers);
	void drawBlurDepth();
	void drawComposite(ID3D11ShaderResourceView* sceneMap);

	FluidRendererD3D11(Graphics*& gfx):
		m_device(nullptr),
		m_deviceContext(nullptr),
		gfx(gfx)
	{}

	void _createScreenQuad();
	void setNumberOfParticles(int nrOfParticles);
	int m_nrOfParticles;

	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;

	ID3D11InputLayout* m_pointThicknessLayout;
	ID3D11VertexShader* m_pointThicknessVs;
	ID3D11GeometryShader* m_pointThicknessGs;
	ID3D11PixelShader* m_pointThicknessPs;

	ID3D11InputLayout* m_ellipsoidDepthLayout;
	ID3D11VertexShader* m_ellipsoidDepthVs;
	ID3D11GeometryShader* m_ellipsoidDepthGs;
	ID3D11PixelShader* m_ellipsoidDepthPs;

	ID3D11InputLayout* m_passThroughLayout;
	ID3D11VertexShader* m_passThroughVs;

	ID3D11PixelShader* m_blurDepthPs;
	ID3D11PixelShader* m_compositePs;

	ID3D11Buffer* m_constantBuffer;
	Graphics* gfx;

	// Right handed rasterizer state
	ID3D11RasterizerState* m_rasterizerState[2][2];

	ID3D11Buffer* m_quadVertexBuffer;
	ID3D11Buffer* m_quadIndexBuffer;

	RenderTargetD3D11 m_thicknessTexture;
	RenderTargetD3D11 m_depthTexture;
	RenderTargetD3D11 m_depthSmoothTexture;

	int m_sceneWidth;
	int m_sceneHeight;

	void calcFluidConstantBuffer(FluidShaderConst& constBuf);
};


