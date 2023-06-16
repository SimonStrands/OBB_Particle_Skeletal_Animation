#include "FluidRenderer.h"



void FluidRenderer::init(Graphics*& gfx, ID3D11Buffer* particles, const int nrOfParticles)
{
	this->gfx = gfx;
	this->particles = particles;
	this->nrOfParticles = nrOfParticles;

	/*
	//change so depth is 1 float???
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = gfx->getWH().x;
	textureDesc.Height = gfx->getWH().y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	if (FAILED(gfx->getDevice()->CreateTexture2D(&textureDesc, NULL, &m_thicknessTexture_T))||
		FAILED(gfx->getDevice()->CreateTexture2D(&textureDesc, NULL, &m_depthTexture_T))||
		FAILED(gfx->getDevice()->CreateTexture2D(&textureDesc, NULL, &m_depthSmoothTexture_T))) {
		printf("doesn't work tex2d");
		return;
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	if (FAILED(gfx->getDevice()->CreateRenderTargetView(m_thicknessTexture_T, &renderTargetViewDesc, &RenderTargetView[0]))||
		FAILED(gfx->getDevice()->CreateRenderTargetView(m_depthTexture_T, &renderTargetViewDesc, &RenderTargetView[1]))||
		FAILED(gfx->getDevice()->CreateRenderTargetView(m_depthSmoothTexture_T, &renderTargetViewDesc, &RenderTargetView[2]))) {
		printf("doesn't work RTV");
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	if (FAILED(gfx->getDevice()->CreateShaderResourceView(m_thicknessTexture_T, &shaderResourceViewDesc, &m_thicknessTexture_SRV))||
		FAILED(gfx->getDevice()->CreateShaderResourceView(m_depthTexture_T, &shaderResourceViewDesc, &m_depthTexture_SRV))||
		FAILED(gfx->getDevice()->CreateShaderResourceView(m_depthSmoothTexture_T, &shaderResourceViewDesc, &m_depthSmoothTexture_SRV)))
	{
		printf("failed create RSV");
		return;
	}

	ID3D11Texture2D* depthStencilTex = nullptr;
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc{};
	depthStencilBufferDesc.Width = (UINT)gfx->getWH().x;
	depthStencilBufferDesc.Height = (UINT)gfx->getWH().y;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	if (FAILED(gfx->getDevice()->CreateTexture2D(&depthStencilBufferDesc, NULL, &depthStencilTex))) 
	{
		printf("failed create tex2d depth");
	}
	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc{};
	DSVDesc.Format = depthStencilBufferDesc.Format;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	if (FAILED(gfx->getDevice()->CreateDepthStencilView(depthStencilTex, &DSVDesc, &this->m_thicknessTexture_DS))||
		FAILED(gfx->getDevice()->CreateDepthStencilView(depthStencilTex, &DSVDesc, &this->m_depthTexture_DS))||
		FAILED(gfx->getDevice()->CreateDepthStencilView(depthStencilTex, &DSVDesc, &this->m_depthSmoothTexture_DS))) {
		printf("DEPTH STENCIL VIEW");
		return ;
	}

	depthStencilTex->Release();	

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

		gfx->getDevice()->CreateBuffer(&bufDesc, &data, &QuadIndeciesBuffer);
	}

	// create a vertex buffer
	{
		
		D3D11_BUFFER_DESC bufDesc;
		bufDesc.ByteWidth = 4*sizeof(vertex);
		bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufDesc.CPUAccessFlags = DXGI_CPU_ACCESS_NONE;
		bufDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data = { 0 };


		std::vector<vertex> vertices;
		std::array<float, 3> pos;
		std::array<float, 2> UV;
		std::array<float, 4> Normal;

		pos = {-1.0f, -1.0f, 0.0f};
		UV = {0.0f, 0.0f};
		Normal = {0, 1, 0, 1};
		vertices.push_back(vertex(pos, UV, Normal));
		pos = {1.0f, -1.0f, 0.0f};
		UV = {1.0f, 0.0f};
		vertices.push_back(vertex(pos, UV, Normal));
		pos = {1.0f,  1.0f, 0.0f};
		UV = {1.0f, 1.0f};
		vertices.push_back(vertex(pos, UV, Normal));
		pos = {-1.0f,  1.0f, 0.0f};
		UV = {0.0f, 1.0f};
		vertices.push_back(vertex(pos, UV, Normal));

		data.pSysMem = vertices.data();

		gfx->getDevice()->CreateBuffer(&bufDesc, &data, &QuadVertexBuffer);
	}
	*/

D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = gfx->getWH().x;
	textureDesc.Height = gfx->getWH().y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	if (FAILED(gfx->getDevice()->CreateTexture2D(&textureDesc, NULL, &m_Fluid_T))) {
		printf("doesn't work tex2d");
		return;
	}
	if (FAILED(gfx->getDevice()->CreateTexture2D(&textureDesc, NULL, &m_Show_T))) {
		printf("doesn't work tex2d");
		return;
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	if (FAILED(gfx->getDevice()->CreateRenderTargetView(m_Fluid_T, &renderTargetViewDesc, &RenderTargetView))) {
		printf("doesn't work RTV");
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	if (FAILED(gfx->getDevice()->CreateShaderResourceView(m_Fluid_T, &shaderResourceViewDesc, &m_Fluid_SRV)))
	{
		printf("failed create RSV");
		return;
	}
	if (FAILED(gfx->getDevice()->CreateShaderResourceView(m_Show_T, &shaderResourceViewDesc, &m_Show_SRV)))
	{
		printf("failed create RSV");
		return;
	}

	ID3D11Texture2D* depthStencilTex = nullptr;
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc{};
	depthStencilBufferDesc.Width = (UINT)gfx->getWH().x;
	depthStencilBufferDesc.Height = (UINT)gfx->getWH().y;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	if (FAILED(gfx->getDevice()->CreateTexture2D(&depthStencilBufferDesc, NULL, &depthStencilTex))) 
	{
		printf("failed create tex2d depth");
		return;
	}
	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc{};
	DSVDesc.Format = depthStencilBufferDesc.Format;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	if (FAILED(gfx->getDevice()->CreateDepthStencilView(depthStencilTex, &DSVDesc, &this->m_Fluid_DS))) {
		printf("DEPTH STENCIL VIEW");
		return;
	}

	m_FluidVs = gfx->getVS()[4];
	m_FluidGs = gfx->getGS()[0];
	m_FluidPs = gfx->getPS()[4];
	loadCShader("Blurer.cso", gfx->getDevice(), m_blurComputeShader);
	std::string a;
	loadVShader("DrawScreenQuad.cso", gfx->getDevice(), m_QuadVs, a);
	loadPShader("ScreenQuadPS.cso", gfx->getDevice(), m_QuadPs);

	HRESULT hr = gfx->getDevice()->CreateUnorderedAccessView(m_Show_T, nullptr, &UAV);
	if(hr!= S_OK){
		printf("UAV");
		return;
	}
}

FluidRenderer::FluidRenderer()
{
}

void FluidRenderer::draw()
{
	//draw the particles on a texture //TODO: should I have the original depth buffer?
	gfx->get_IMctx()->OMSetRenderTargets(1, &RenderTargetView, gfx->getDepthStencil());
	gfx->get_IMctx()->VSSetShader(m_FluidVs, nullptr, 0);
	gfx->get_IMctx()->GSSetShader(m_FluidGs, nullptr, 0);
	gfx->get_IMctx()->PSSetShader(m_FluidPs, nullptr, 0);
	gfx->get_IMctx()->HSSetShader(nullptr, nullptr, 0);
	gfx->get_IMctx()->DSSetShader(nullptr, nullptr, 0);

	gfx->get_IMctx()->Draw(this->nrOfParticles, 0);

	//blur the texture
	gfx->get_IMctx()->CSSetShader(m_blurComputeShader, nullptr, 0);
	gfx->get_IMctx()->CSSetShaderResources(0, 1, &m_Fluid_SRV);
	
	gfx->get_IMctx()->CSSetUnorderedAccessViews(0, 1, &this->UAV, nullptr);
	//köra computeShader
	gfx->get_IMctx()->Dispatch(60, 135, 1);
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	gfx->get_IMctx()->CSSetShaderResources(0, 1, nullSRV);
	//nulla unorderedaccesview
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	gfx->get_IMctx()->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);

	//draw the texture on a quad on the camera
	gfx->setRenderTarget();

	//TODO : CHANGE THIS SO WE CAN Create the ScreenQuad
	gfx->get_IMctx()->VSSetShader(m_FluidVs, nullptr, 0);
	gfx->get_IMctx()->GSSetShader(nullptr, nullptr, 0);
	gfx->get_IMctx()->PSSetShader(m_FluidPs, nullptr, 0);


	float clearColor[4] = {0,0,0,0};
	gfx->get_IMctx()->ClearRenderTargetView(RenderTargetView, clearColor);

}
