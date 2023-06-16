#pragma once
#include "Graphics.h"
#include <d3d11.h>

class FluidRenderer{
public:
	FluidRenderer();
	void init(Graphics*& gfx, ID3D11Buffer* particles, const int nrOfParticles);
	void draw();
private:
	Graphics* gfx;
	ID3D11Buffer* particles;
	int nrOfParticles;

	//screenQuad
	ID3D11Buffer* QuadVertexBuffer;
	ID3D11Buffer* QuadIndeciesBuffer;

	ID3D11Texture2D* m_Fluid_T;
	ID3D11DepthStencilView* m_Fluid_DS;
	ID3D11ShaderResourceView* m_Fluid_SRV;

	ID3D11Texture2D* m_Show_T;
	ID3D11ShaderResourceView* m_Show_SRV;

	ID3D11RenderTargetView* RenderTargetView;//thicc, depth, smooth

	ID3D11VertexShader* m_FluidVs;
	ID3D11GeometryShader* m_FluidGs;
	ID3D11PixelShader* m_FluidPs;
	ID3D11ComputeShader* m_blurComputeShader;
	ID3D11VertexShader* m_QuadVs;
	ID3D11PixelShader* m_QuadPs;

	ID3D11UnorderedAccessView* UAV;//till Compute shadern
	/*
	ID3D11Texture2D* m_thicknessTexture_T;
	ID3D11DepthStencilView* m_thicknessTexture_DS;
	ID3D11ShaderResourceView* m_thicknessTexture_SRV;

	ID3D11Texture2D* m_depthTexture_T;
	ID3D11DepthStencilView* m_depthTexture_DS;
	ID3D11ShaderResourceView* m_depthTexture_SRV;

	ID3D11Texture2D* m_depthSmoothTexture_T;
	ID3D11DepthStencilView* m_depthSmoothTexture_DS;
	ID3D11ShaderResourceView* m_depthSmoothTexture_SRV;

	ID3D11RenderTargetView* RenderTargetView[3];//thicc, depth, smooth
	*/
};