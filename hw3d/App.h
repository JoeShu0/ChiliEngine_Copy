#pragma once
#include "Window.h"
#include "CustomTimer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "TestPlane.h"
#include "Mesh.h"
#include <set>

class App
{
public:
	//App();
	App(const std::string& commandLine = "");
	// master frame / message loop
	int Go();
	~App();
private:
	void DoFrame();
	void MoveCamera(float dt);
	void ShowImguiDemoWindow();
	//void ShowRawInputWindow();

private:
	std::string commandLine;
	//int x = 0, y = 0;
	ImguiManager imgui;
	Window wnd;
	CustomTimer timer;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	std::vector<class Box*> boxes;
	//bool show_demo_window = true;
	float speed_factor = 1.0f;
	//global camera
	Camera cam;
	bool IsMoveingCam = false;
	//global light
	PointLight light;
	//float MousePosX, MousePosY;
	static constexpr size_t nDrawables = 1001;
	std::optional<int> comboBoxIndex;
	std::set<int> boxControlIds;
	void SpawnSimulationWindow() noexcept;
	//Model nano{ wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj", 1.0f };
	//Model nano2{ wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj" };
	//TestPlane plane;
	//Model wall{ wnd.Gfx(), "Models\\brick_wall\\brick_wall.obj", 6.0f};
	//Model gobber{ wnd.Gfx(), "Models\\gobber\\GoblinX.obj", 6.0f};
	//TestPlane tp{ wnd.Gfx(),6.0 };
	Model sponza{ wnd.Gfx(), "Models\\sponza\\sponza.obj", 1.0f / 20.0f };
	TestPlane bluePlane{ wnd.Gfx(),6.0f,{ 0.1f,0.1f,0.9f,0.0f } };
	TestPlane redPlane{ wnd.Gfx(),6.0f,{ 0.9f,0.1f,0.1f,0.0f } };
};