/*
 * Copyright (c) 2008-2017, NVIDIA CORPORATION.  All rights reserved.
 *
 * NVIDIA CORPORATION and its licensors retain all intellectual property
 * and proprietary rights in and to this software, related documentation
 * and any modifications thereto.  Any use, reproduction, disclosure or
 * distribution of this software and related documentation without an express
 * license agreement from NVIDIA CORPORATION is strictly prohibited.
 */

//direct3d headers

#include "fluidRenderD3D11.h"

#include <math.h>


void FluidRendererD3D11::calcFluidConstantBuffer(FluidShaderConst& constBuf)
{	
	//no model matrix stuck at 0,0,0
	constBuf.modelViewProjection = (DirectX::XMFLOAT4X4&)(DirectX::XMMatrixMultiply(gfx->getVertexconstbuffer()->view.element, gfx->getVertexconstbuffer()->projection.element));
	constBuf.modelView = (DirectX::XMFLOAT4X4&)gfx->getVertexconstbuffer()->view.element;
	constBuf.projection = (DirectX::XMFLOAT4X4&)gfx->getVertexconstbuffer()->projection.element;
	constBuf.inverseModelView = (DirectX::XMFLOAT4X4&)DirectX::XMMatrixInverse(nullptr, gfx->getVertexconstbuffer()->view.element);
	constBuf.inverseProjection = (DirectX::XMFLOAT4X4&)DirectX::XMMatrixInverse(nullptr, gfx->getVertexconstbuffer()->projection.element);

	constBuf.invViewport = gfx->getInvViewPort();

	//constBuf.blurRadiusWorld = params.blurRadiusWorld;
	constBuf.blurRadiusWorld = 0.25f;
	//constBuf.blurScale = params.blurScale;
	constBuf.blurScale = gfx->getWH().x / (16/9) * (1.0f/std::tan(90 * 0.5f));
	constBuf.blurFalloff = 1.0f;
	constBuf.debug = false;

	constBuf.pointRadius = 0.065f;
}

struct PassthroughVertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 texcoords;
	DirectX::XMFLOAT4 color;
};

void FluidRendererD3D11::init(ID3D11Device* device, ID3D11DeviceContext* context, int width, int height)
{
	std::string VShaderByteCode[3];
	loadVShader("ellipsoidDepthVS.cso", device, this->m_ellipsoidDepthVs, VShaderByteCode[0]);
	loadGShader("ellipsoidDepthGS.cso", device, this->m_ellipsoidDepthGs);
	loadPShader("ellipsoidDepthPS.cso", device, this->m_ellipsoidDepthPs);
	loadVShader("pointThicknessVS.cso", device, this->m_pointThicknessVs, VShaderByteCode[1]);
	loadGShader("pointThicknessGS.cso", device, this->m_pointThicknessGs);
	loadPShader("pointThicknessPS.cso", device, this->m_pointThicknessPs);
	loadVShader("passThroughVS.cso", device, this->m_passThroughVs, VShaderByteCode[2]);
	loadPShader("blurDepthPS.cso", device, this->m_blurDepthPs);
	loadPShader("compositePS.cso", device, this->m_compositePs);

	m_sceneWidth = width;
	m_sceneHeight = height;

	m_thicknessTexture.init(device, width, height);
	m_depthTexture.init(device, width, height);
	m_depthSmoothTexture.init(device, width, height, false);

	m_device = device;
	m_deviceContext = context;

	// create the input layout
	{
		D3D11_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "U", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "V", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "W", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		m_device->CreateInputLayout(inputElementDescs, 4, VShaderByteCode[1].c_str(), VShaderByteCode[1].length(), &m_pointThicknessLayout);
	}
	
	// create the input layout
	{
		D3D11_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "U", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "V", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "W", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		m_device->CreateInputLayout(inputElementDescs, 4, VShaderByteCode[0].c_str(), VShaderByteCode[0].length(), &m_ellipsoidDepthLayout);
	}

	// create the input layout
	{
		D3D11_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		m_device->CreateInputLayout(inputElementDescs, 4, VShaderByteCode[2].c_str(), VShaderByteCode[2].length(), &m_passThroughLayout);
	}

	// create a constant buffer
	{
		D3D11_BUFFER_DESC bufDesc;
		bufDesc.ByteWidth = sizeof(FluidShaderConst); // 64 * sizeof(float);
		bufDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufDesc.MiscFlags = 0;

		m_device->CreateBuffer(&bufDesc, nullptr, &m_constantBuffer);
	}

	// create the rastersizer state
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			D3D11_RASTERIZER_DESC desc = {};
			desc.FillMode = (D3D11_FILL_MODE)(D3D11_FILL_WIREFRAME + i);
			desc.CullMode = (D3D11_CULL_MODE)(D3D11_CULL_NONE + j);
			desc.FrontCounterClockwise = TRUE;	// This is non-default
			desc.DepthBias = 0;
			desc.DepthBiasClamp = 0.f;
			desc.SlopeScaledDepthBias = 0.f;
			desc.DepthClipEnable = TRUE;
			desc.ScissorEnable = FALSE;
			desc.MultisampleEnable = FALSE;
			desc.AntialiasedLineEnable = FALSE;

			m_device->CreateRasterizerState(&desc, &m_rasterizerState[i][j]);
		}
	}

	_createScreenQuad();
}

void FluidRendererD3D11::_createScreenQuad()
{
	// create an index buffer
	{
		D3D11_BUFFER_DESC bufDesc;
		bufDesc.ByteWidth = 4*sizeof(UINT);
		bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufDesc.CPUAccessFlags = DXGI_CPU_ACCESS_NONE;
		bufDesc.MiscFlags = 0;

		unsigned int quad_indices[4] = { 0, 1, 3, 2 };
		
		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = quad_indices;

		m_device->CreateBuffer(&bufDesc, &data, &m_quadIndexBuffer);
	}

	// create a vertex buffer
	{
		
		D3D11_BUFFER_DESC bufDesc;
		bufDesc.ByteWidth = 4*sizeof(PassthroughVertex);
		bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufDesc.CPUAccessFlags = DXGI_CPU_ACCESS_NONE;
		bufDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data = { 0 };


		PassthroughVertex vertices[4] =
		{
		 	{ {-1.0f, -1.0f, 0.0f},  {0, 1, 0}, {0.0f, 0.0f}, {1, 1, 1, 1}},
			{ { 1.0f, -1.0f, 0.0f},  {0, 1, 0}, {1.0f, 0.0f}, {1, 1, 1, 1}},
			{ { 1.0f,  1.0f, 0.0f},  {0, 1, 0}, {1.0f, 1.0f}, {1, 1, 1, 1}},
			{ {-1.0f,  1.0f, 0.0f},  {0, 1, 0}, {0.0f, 1.0f}, {1, 1, 1, 1}},
		};

		data.pSysMem = vertices;

		m_device->CreateBuffer(&bufDesc, &data, &m_quadVertexBuffer);
	}
}

void FluidRendererD3D11::setNumberOfParticles(int nrOfParticles)
{
	this->m_nrOfParticles = nrOfParticles;
}


void FluidRendererD3D11::drawThickness(const FluidRenderBuffersD3D11* buffers)
{
	ID3D11DeviceContext* deviceContext = m_deviceContext;

	// update constant buffer
	{

		D3D11_BUFFER_DESC desc;
		m_constantBuffer->GetDesc(&desc);

		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		if (deviceContext->Map(m_constantBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource) == S_OK)
		{
			FluidShaderConst constBuf;
			calcFluidConstantBuffer(constBuf);
			memcpy(mappedResource.pData, &constBuf, sizeof(FluidShaderConst));
			deviceContext->Unmap(m_constantBuffer, 0u);
		}
	}

	deviceContext->VSSetShader(m_pointThicknessVs, nullptr, 0u);
	deviceContext->GSSetShader(m_pointThicknessGs, nullptr, 0u);
	deviceContext->PSSetShader(m_pointThicknessPs, nullptr, 0u);

	deviceContext->IASetInputLayout(m_pointThicknessLayout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	deviceContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);
	deviceContext->GSSetConstantBuffers(0, 1, &m_constantBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &m_constantBuffer);

	ID3D11Buffer* vertexBuffers[4] =
	{
		buffers->m_positions,
		buffers->m_anisotropiesArr[0],
		buffers->m_anisotropiesArr[1],
		buffers->m_anisotropiesArr[2]
	};

	unsigned int vertexBufferStrides[4] =
	{
		sizeof(DirectX::XMFLOAT4),
		sizeof(DirectX::XMFLOAT4),
		sizeof(DirectX::XMFLOAT4),
		sizeof(DirectX::XMFLOAT4)
	};

	unsigned int vertexBufferOffsets[4] = { 0 };

	deviceContext->IASetVertexBuffers(0, 4, vertexBuffers, vertexBufferStrides, vertexBufferOffsets);
	deviceContext->IASetIndexBuffer(buffers->m_indices, DXGI_FORMAT_R32_UINT, 0u);

	//float depthSign = DirectX::XMVectorGetW(params->projection.r[2]);
	float depthSign = DirectX::XMVectorGetW(gfx->getVertexconstbuffer()->projection.element.r[2]);
	if (depthSign < 0.f)
	{
		deviceContext->RSSetState(m_rasterizerState[1][0]);
	}

	//n is the number of particles
	deviceContext->DrawIndexed(params->n, params->offset, 0);

	if (depthSign < 0.f)
	{
		deviceContext->RSSetState(nullptr);
	}
}

void FluidRendererD3D11::drawEllipsoids(const FluidRenderBuffersD3D11* buffers)
{
	ID3D11DeviceContext* deviceContext = m_deviceContext;

	// update constant buffer
	{

		D3D11_BUFFER_DESC desc;
		m_constantBuffer->GetDesc(&desc);

		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		if (deviceContext->Map(m_constantBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource) == S_OK)
		{
			FluidShaderConst constBuf;
			calcFluidConstantBuffer(constBuf);
			memcpy(mappedResource.pData, &constBuf, sizeof(FluidShaderConst));
			deviceContext->Unmap(m_constantBuffer, 0u);
		}
	}

	deviceContext->VSSetShader(m_ellipsoidDepthVs, nullptr, 0u);
	deviceContext->GSSetShader(m_ellipsoidDepthGs, nullptr, 0u);
	deviceContext->PSSetShader(m_ellipsoidDepthPs, nullptr, 0u);

	deviceContext->IASetInputLayout(m_ellipsoidDepthLayout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	deviceContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);
	deviceContext->GSSetConstantBuffers(0, 1, &m_constantBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &m_constantBuffer);

	ID3D11Buffer* vertexBuffers[4] =
	{
		buffers->m_positions,
		buffers->m_anisotropiesArr[0],
		buffers->m_anisotropiesArr[1],
		buffers->m_anisotropiesArr[2]
	};

	unsigned int vertexBufferStrides[4] =
	{
		sizeof(DirectX::XMFLOAT4),
		sizeof(DirectX::XMFLOAT4),
		sizeof(DirectX::XMFLOAT4),
		sizeof(DirectX::XMFLOAT4)
	};

	unsigned int vertexBufferOffsets[4] = { 0 };

	deviceContext->IASetVertexBuffers(0, 4, vertexBuffers, vertexBufferStrides, vertexBufferOffsets);
	deviceContext->IASetIndexBuffer(buffers->m_indices, DXGI_FORMAT_R32_UINT, 0u);

	float depthSign = DirectX::XMVectorGetW(gfx->getVertexconstbuffer()->projection.element.r[2]);
	if (depthSign < 0.f)
	{
		deviceContext->RSSetState(m_rasterizerState[1][0]);
	}

	//n is the number of particles
	deviceContext->DrawIndexed(params->n, params->offset, 0);

	if (depthSign < 0.f)
	{
		deviceContext->RSSetState(nullptr);
	}
}

void FluidRendererD3D11::drawBlurDepth()
{
	ID3D11DeviceContext* deviceContext = m_deviceContext;

	// update constant buffer
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		if (deviceContext->Map(m_constantBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource) == S_OK)
		{
			FluidShaderConst constBuf;
			calcFluidConstantBuffer(constBuf);
			memcpy(mappedResource.pData, &constBuf, sizeof(FluidShaderConst));
			deviceContext->Unmap(m_constantBuffer, 0u);
		}
	}

	deviceContext->VSSetShader(m_passThroughVs, nullptr, 0u);
	deviceContext->GSSetShader(nullptr, nullptr, 0u);
	deviceContext->PSSetShader(m_blurDepthPs, nullptr, 0u);

	ID3D11ShaderResourceView* srvs[2] = { m_depthTexture.m_backSrv.Get(), m_thicknessTexture.m_backSrv.Get() };
	deviceContext->PSSetShaderResources(0, 2, srvs);

	ID3D11SamplerState* samps[1] = { m_thicknessTexture.m_linearSampler.Get() };
	deviceContext->PSSetSamplers(0, 1, samps);

	deviceContext->IASetInputLayout(m_passThroughLayout);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	deviceContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &m_constantBuffer);

	UINT vertexStride = sizeof(PassthroughVertex);
	UINT offset = 0u;
	deviceContext->IASetVertexBuffers(0, 1, &m_quadVertexBuffer, &vertexStride, &offset);
	deviceContext->IASetIndexBuffer(m_quadIndexBuffer, DXGI_FORMAT_R32_UINT, 0u);

	float depthSign = DirectX::XMVectorGetW(gfx->getVertexconstbuffer()->projection.element.r[2]);
	if (depthSign < 0.f)
	{
		deviceContext->RSSetState(m_rasterizerState[1][0]);
	}

	deviceContext->DrawIndexed((UINT)4, 0, 0);

	if (depthSign < 0.f)
	{
		deviceContext->RSSetState(nullptr);
	}
}

void FluidRendererD3D11::drawComposite(ID3D11ShaderResourceView* sceneMap)
{
	ID3D11DeviceContext* deviceContext = m_deviceContext;

	// update constant buffer
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource = {};
		if (deviceContext->Map(m_constantBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource) == S_OK)
		{
			FluidShaderConst constBuf;
			RenderParamsUtilD3D::calcFluidCompositeConstantBuffer(*params, constBuf);
			memcpy(mappedResource.pData, &constBuf, sizeof(FluidShaderConst));
			deviceContext->Unmap(m_constantBuffer, 0u);
		}
	}

	deviceContext->VSSetShader(m_passThroughVs.Get(), nullptr, 0u);
	deviceContext->GSSetShader(nullptr, nullptr, 0u);
	deviceContext->PSSetShader(m_compositePs.Get(), nullptr, 0u);

	RenderTargetD3D11* depthMap = &m_depthSmoothTexture;
	ShadowMapD3D11* shadowMap = (ShadowMapD3D11*)params->shadowMap;

	ID3D11ShaderResourceView* srvs[3] = 
	{ 
		depthMap->m_backSrv.Get(),
		sceneMap,
		shadowMap->m_depthSrv.Get()

	};
	deviceContext->PSSetShaderResources(0, 3, srvs);

	ID3D11SamplerState* samps[2] = 
	{ 
		depthMap->m_linearSampler.Get() ,
		shadowMap->m_linearSampler.Get()
	};
	deviceContext->PSSetSamplers(0, 2, samps);

	deviceContext->IASetInputLayout(m_passThroughLayout.Get());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	deviceContext->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

	UINT vertexStride = sizeof(PassthroughVertex);
	UINT offset = 0u;
	deviceContext->IASetVertexBuffers(0, 1, m_quadVertexBuffer.GetAddressOf(), &vertexStride, &offset);
	deviceContext->IASetIndexBuffer(m_quadIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);

	float depthSign = DirectX::XMVectorGetW(params->projection.r[2]);
	if (depthSign < 0.f)
	{
		deviceContext->RSSetState(m_rasterizerState[1][0]);
	}

	deviceContext->DrawIndexed((UINT)4, 0, 0);

	if (depthSign < 0.f)
	{
		deviceContext->RSSetState(nullptr);
	}

	// reset srvs
	ID3D11ShaderResourceView* zero[3] = { NULL, NULL, NULL };
	deviceContext->PSSetShaderResources(0, 3, zero);
}

