#include "Game.h"

//git
Game::Game(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	gfx = new Graphics(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	defRend = new DeferredRendering(gfx);
	//Create a buffer for the light const buffer(hlsli)
	CreateConstBuffer(gfx, gfx->getConstBuffers(0), sizeof(*gfx->getLightconstbufferforCS()), gfx->getLightconstbufferforCS());
	CreateConstBuffer(gfx, gfx->getConstBuffers(1), sizeof(*gfx->getCamPosconstbuffer()), gfx->getCamPosconstbuffer());
	//Resource manager
	rm = new ResourceManager(gfx);
	
	//create lights
	nrOfLight = 1;
	light = new Light * [nrOfLight];
	//light[0] = new DirLight(vec3(0,60,8), vec3(0.1f, -PI/2, 1.f), 50,50);
	//light[0] = new SpotLight(vec3(0, 46, 0), vec3(0.f, -3.14f/2.f, 0.f));
	light[0] = new SpotLight(vec3(0, 12, 7.0f), vec3(0.f, -3.14f/2.f, 0.f));
	//light[2] = new SpotLight(vec3(8, 47.f, 0), vec3(0, -1, 1));
	//light[3] = new SpotLight(vec3(30, 50, 0), vec3(-1, -1, 1));
	gfx->getLightconstbufferforCS()->nrOfLights.element = nrOfLight;
	
	//shadow map needs to take more lights
	this->shadowMap = new ShadowMap((SpotLight**)light, nrOfLight, gfx);
	
	gfx->takeIM(&this->UIManager);
	mus = new Mouse(gfx->getWH());
	camera = new Camera(gfx, mus, vec3(-10,0,0), vec3(1,0,0));
	camera->setData();
	
	setUpObject();
	
	/////LIGHT////////
	for (int i = 0; i < nrOfLight; i++) {
		LightVisualizers.push_back(new GameObject(rm->get_Models("Camera.obj"), gfx, light[i]->getPos(), light[i]->getRotation(), vec3(1.f, 1.0f, 1.0f)));
	}
	//UI
	//light
	for (int i = 0; i < nrOfLight; i++) {
		UIManager.takeLight(light[i]);
	}
	
	gfx->takeLight((SpotLight**)light, nrOfLight);
	
	lightNr = 0;

}

Game::~Game() 
{
	//part of game
 	TC::GetInst().empty();
	delete gfx;
	delete rm;

	//logic and other
	delete defRend;
	delete mus;
	delete camera;
	if (shadowMap != nullptr) {
		delete shadowMap;
	}

	//objects
	for (int i = 0; i < LightVisualizers.size(); i++) {
		delete light[i];
		delete LightVisualizers[i];
	}
	delete[] light;
	for (int i = 0; i < obj.size(); i++) {
		delete obj[i];
	}
}


void Game::run()
{
	static bool once = true;
	while (msg.message != WM_QUIT && once)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		gfx->clearScreen();
		gfx->setTransparant(false);
		//for shadow
		//måste uppdatera detta så inte hela object uppdateras när bara skugga ska
		shadowMap->setUpdateShadow();
		vec3 camLP = camera->getPos();
		vec3 camLR = camera->getRot();
		for (int i = 0; i < nrOfLight; i++) {
			//set cam position to lightposition
			camera->setPosition(light[i]->getPos());
			camera->setRotation(light[i]->getRotation());
			shadowMap->inUpdateShadow(i);
			updateShaders(true, false);
			
			DrawAllShadowObject();
		}
		//set cam position so its the real cam
		camera->setPosition(camLP);
		camera->setRotation(camLR);
		gfx->setProjection(0);//last can be dir light
		gfx->RsetViewPort();


		Update();
		updateShaders();
		
		defRend->BindFirstPass();
		
		this->DrawToBuffer();
		
		defRend->BindSecondPass(shadowMap->GetshadowResV());

		gfx->get_IMctx()->PSSetShaderResources(5, 1, &shadowMap->GetshadowResV());

		gfx->setTransparant(true);
		gfx->setRenderTarget();
		this->ForwardDraw();
		gfx->present(this->lightNr, (float)dt.dt());

	}
	printf("quit");
}

void Game::Update()
{
	dt.restartClock();
	//keyboard
	camera->updateCamera((float)dt.dt());
	if (getkey('N')) {
		DirectX::XMMATRIX viewMatrix = DirectX::XMMATRIX(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			-obj[1]->getPos().x, -obj[1]->getPos().y, -obj[1]->getPos().z, 1.0f
		);
		XRotation(viewMatrix, obj[1]->getRot().x);
		YRotation(viewMatrix, obj[1]->getRot().y);
		gfx->getVertexconstbuffer()->view.element = viewMatrix;
	}
	
	obj[1]->setPos(vec3(obj[0]->getPos().x, obj[1]->getPos().y, obj[0]->getPos().z));

	obj[0]->setPos(camera->getPos());
	obj[0]->setRot(vec3(camera->getRot().z, camera->getRot().x, -camera->getRot().y) + vec3(0, 1.57f, 0));

	mus->UpdateMouse();
	for (int i = 0; i < LightVisualizers.size(); i++) {
		LightVisualizers[i]->setPos(light[i]->getPos());
		LightVisualizers[i]->setRot(vec3(0 , light[i]->getRotation().x, -light[i]->getRotation().y) + vec3(0,1.57f,0));
	}
	static bool onceUpdate = false;
	if(onceUpdate){
		particleModel.updateParticles((float)dt.dt(), gfx);
	}
	onceUpdate = true;
	

	gfx->Update((float)dt.dt(), camera->getPos());

#pragma region camera_settings
	if (getkey('C')) {
		camera->setPosition(light[lightNr]->getPos());
		camera->setRotation(light[lightNr]->getRotation());
	}
	if (getkey('1')) {
		lightNr = 0;
	}
	if (getkey('2')) {
		lightNr = 1;
	}
	if (getkey('3')) {
		lightNr = 2;
	}
	if (getkey('4')) {
		lightNr = 3;
	}
#pragma endregion camera_settings
}

void Game::DrawToBuffer()
{	
	gfx->get_IMctx()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gfx->get_IMctx()->IASetInputLayout(gfx->getInputLayout()[0]);
	gfx->get_IMctx()->GSSetShader(nullptr, nullptr, 0);
	gfx->get_IMctx()->VSSetShader(gfx->getVS()[0], nullptr, 0);
	gfx->get_IMctx()->HSSetShader(nullptr, nullptr, 0);
	gfx->get_IMctx()->DSSetShader(nullptr, nullptr, 0);
	for (int i = 0; i < obj.size(); i++) {
		obj[i]->draw(gfx);
	}
    camera->calcFURVectors();

	gfx->get_IMctx()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gfx->get_IMctx()->VSSetShader(gfx->getVS()[0], nullptr, 0);
	gfx->get_IMctx()->HSSetShader(nullptr, nullptr, 0);
	gfx->get_IMctx()->DSSetShader(nullptr, nullptr, 0);
	gfx->get_IMctx()->PSSetShader(gfx->getPS()[0], nullptr, 0);
	for (int i = 0; i < LightVisualizers.size(); i++) {
		LightVisualizers[i]->draw(gfx, false);
	}
}

void Game::ForwardDraw()
{
	particleModel.draw(gfx);
}

void Game::DrawAllShadowObject()
{
	gfx->get_IMctx()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gfx->get_IMctx()->GSSetShader(nullptr, nullptr, 0);
	gfx->get_IMctx()->PSSetShader(nullptr, nullptr, 0);
	for (int i = 0; i < obj.size(); i++) {
		obj[i]->draw(gfx, true);
	}
	particleModel.drawShadow(gfx);
	camera->calcFURVectors();
}

void Game::updateShaders(bool vs, bool ps)
{
	if (vs)
	{
		for (int i = 0; i < obj.size(); i++) {
			obj[i]->updateVertexShader(gfx);
		}
		for (int i = 0; i < LightVisualizers.size(); i++) {
			LightVisualizers[i]->updateVertexShader(gfx);
		}
	}
	if (ps) {
		for (int i = 0; i < obj.size(); i++) {
			obj[i]->updatePixelShader(gfx);
		}
		for (int i = 0; i < LightVisualizers.size(); i++) {
			LightVisualizers[i]->updatePixelShader(gfx);
		}
	}
}

void Game::setUpObject()
{
	////////OBJECTS///////////
	//cameras
	obj.push_back(new GameObject(rm->get_Models("Camera.obj", gfx), gfx, vec3(0.f, 0.f, 10.f), vec3(0.f, 0.f, 0.f), vec3(0.01f, 0.01f, 0.01f)));//main
	obj.push_back(new GameObject(rm->get_Models("Camera.obj", gfx), gfx, vec3(0.f, 100.f, 0.f), vec3(0.f, -1.58f, 0.f), vec3(2.f, 2.0f, 2.0f)));//second
	obj.push_back(new GameObject(rm->get_Models("stormtrooper.obj",gfx), gfx, vec3(0,2,0.2f), vec3(0,0,0), vec3(0.2f,0.2f,0.2f)));

	//particleModel.init(gfx, "objects/sillydance2.fbx", vec3());
	particleModel.init(gfx, "objects/StormTrooperFBX.fbx", vec3());
	//particleModel = new ParticleModel(gfx, "objects/test2.fbx", vec3());
	//particleModel = new ParticleModel(gfx, "objects/quad2.obj", vec3());
}
