#pragma once
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <vector>
#include "object.h"
#include "Light.h"
#include <string>

#include <windows.h>
#include "psapi.h"
static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
static int numProcessors;
static HANDLE self;

class ImguiManager {
public:
	ImguiManager();
	~ImguiManager();
	void takeObject(object* obj);
	void takeLight(Light* light);
	void updateRender(int lightNr, float deltaTime);
private:
	std::vector<object*> obj;
	std::vector<Light*> light;

	int frames;
	double avfps;
	double time;
	double frameRate;

	double getCurrentCPUValue();
};