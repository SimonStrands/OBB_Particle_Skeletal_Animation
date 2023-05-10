#include "imguiManager.h"

ImguiManager::ImguiManager()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	this->frameRate = 0;
	this->avfps = 0.f;
}

ImguiManager::~ImguiManager()
{
	ImGui::DestroyContext();
}

void ImguiManager::takeObject(object* obj)
{
	this->obj.push_back(obj);
}

void ImguiManager::takeLight(Light* light)
{
	this->light.push_back(light);
}

void ImguiManager::takeBoxes(std::vector<DirectX::XMFLOAT3>& boxes)
{
	this->boxes = &boxes;
}

void ImguiManager::updateRender(int lightNr, float deltaTime)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	for (int i = 0; i < obj.size(); i++) {
		std::string name = "obj " + std::to_string(i);
		if (ImGui::Begin(name.c_str())) {
			ImGui::SliderFloat("Xpos", &obj[i]->getxPos(), 40.0f, -40.0f);
			ImGui::SliderFloat("Ypos", &obj[i]->getyPos(), 40.0f, -40.0f);
			ImGui::SliderFloat("Zpos", &obj[i]->getzPos(), 40.0f, -40.0f);
			ImGui::SliderFloat("XRot", &obj[i]->getxRot(), 20.0f, -20.0f);
			ImGui::SliderFloat("YRot", &obj[i]->getyRot(), 20.0f, -20.0f);
			ImGui::SliderFloat("ZRot", &obj[i]->getzRot(), 20.0f, -20.0f);
		}
		ImGui::End();
	}
	std::string name = "light" + std::to_string(lightNr);
	if (ImGui::Begin(name.c_str())) {
		ImGui::SliderFloat("Xpos", &light[lightNr]->getPos().x, 60.0f, -60.0f);
		ImGui::SliderFloat("Ypos", &light[lightNr]->getPos().y, 60.0f, -60.0f);
		ImGui::SliderFloat("Zpos", &light[lightNr]->getPos().z, 60.0f, -60.0f);
		ImGui::SliderFloat("XRot", &light[lightNr]->getRotation().x, 6.3f, -6.3f);
		ImGui::SliderFloat("YRot", &light[lightNr]->getRotation().y, 6.3f, -6.3f);
		ImGui::SliderFloat("ZRot", &light[lightNr]->getRotation().z, 6.3f, -6.3f);
	}
	ImGui::End();

	if(boxes != nullptr){
		static int boxnr = 0;
		
		if(ImGui::Begin("boxes")){
			 ImGui::SliderInt("WhatBox", &boxnr, 0, boxes->size() - 1);
			 ImGui::SliderFloat("height", &(boxes[0][boxnr]).x, 0.f, 5.f);
			 ImGui::SliderFloat("width", &(boxes[0][boxnr]).y, 0.f, 5.f);
			 ImGui::SliderFloat("depth", &(boxes[0][boxnr]).z, 0.f, 5.f);
			
		}
		ImGui::End();
	}
	
	static bool startSpecTesting = false;
	if(getkey('L')){
		startSpecTesting = true;
	}

	ImGui::Begin("Application Specs");
	if(startSpecTesting){
		std::string dtText = "Delta Time: " + std::to_string(deltaTime);
		
		//virtual memory currently used by the process
		PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
		std::string virtualMemUsedByMe = "Virtual: " + std::to_string(pmc.PrivateUsage / (1024 * 1024))+" MB";

		//Physical Memory currently used by current process:
		std::string physMemUsedByMe = "Physical: " + std::to_string(pmc.WorkingSetSize / (1024 * 1024))+" MB";

		frames++;
		time += deltaTime;
		if (time >= 1.f)
		{
			frameRate = frames;
			if (avfps == 0.f)
				avfps = frameRate;
			else
				avfps = (avfps + frameRate) / 2;
			frames = 0;
			time = 0;
		}

		if(minimumFPS < deltaTime){
			minimumFPS = deltaTime;
		}
		std::string minFPSStr = "Min FPS: " + std::to_string(1.0/minimumFPS);

		if(maxFPS > deltaTime){
			maxFPS = deltaTime;
		}
		std::string maxFPSStr = "Max FPS: " + std::to_string(1.0/maxFPS);

		std::string fps = std::to_string(frameRate)+ " FPS" ;
		std::string afps = "Avarage FPS for entire runtime: " + std::to_string(avfps);

		ImGui::Text(dtText.c_str());
		ImGui::Text(fps.c_str());
		ImGui::Text(afps.c_str());
		ImGui::Text(minFPSStr.c_str());
		ImGui::Text(maxFPSStr.c_str());
		ImGui::Text("Used memory by app");
		ImGui::Text(virtualMemUsedByMe.c_str());
		ImGui::Text(physMemUsedByMe.c_str());
	}
	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


