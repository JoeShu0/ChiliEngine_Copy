#include "App.h"
#include <memory>
#include <algorithm>
#include "CustomMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include "VertexBuffer.h"
#include "SkyBox.h"
#include "TexturePreProcessor.h" 
#include <shellapi.h>
#include <dxtex/DirectXTex.h>


//#include "WindowsMessageMap.h"

GDIPlusManager gdipm;
namespace dx = DirectX;


App::App( const std::string& commandLine)
	:
	commandLine(commandLine),
	wnd(1280,720, "D3D11 Engine Window"),
	light(wnd.Gfx()),
	cam(wnd.Gfx(), dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 40000.0f))
{
	/*
	//dxtex test code
	auto scratch = DirectX::ScratchImage{};
	DirectX::LoadFromWICFile(L"Images\\brickwall.jpg", DirectX::WIC_FLAGS_NONE, nullptr, scratch);
	auto image = scratch.GetImage(0, 0, 0);
	auto a = image->pixels[0];
	auto b = image->pixels[1];
	auto c = image->pixels[2];
	auto d = image->pixels[3];
	*/
	//make shift to CLI to do some preprocessing stuff
	if (this->commandLine != "")
	{
		int nArgs;
		const auto pLineW = GetCommandLineW();
		const auto pArgs = CommandLineToArgvW(pLineW, &nArgs);
		if (nArgs >= 3 && std::wstring(pArgs[1]) == L"--twerk-objnorm")
		{
			int nArgs;
			const auto pLineW = GetCommandLineW();
			const auto pArgs = CommandLineToArgvW(pLineW, &nArgs);
			if (nArgs >= 4 && std::wstring(pArgs[1]) == L"--ntwerk-rotx180")
			{
				const std::wstring pathInWide = pArgs[2];
				TexturePreprocessor::FlipYAllNormalMapsInObj(
					std::string(pathInWide.begin(), pathInWide.end())
				);
				throw std::runtime_error("Normal map processed successfully. Just kidding about that whole runtime error thing.");
			}
			else if (nArgs >= 3 && std::wstring(pArgs[1]) == L"--twerk-flipy")
			{
				const std::wstring pathInWide = pArgs[2];
				const std::wstring pathOutWide = pArgs[3];
				TexturePreprocessor::FlipYNormalMap(
					std::string(pathInWide.begin(), pathInWide.end()),
					std::string(pathOutWide.begin(), pathOutWide.end())
				);
				throw std::runtime_error("Normal map processed successfully. Just kidding about that whole runtime error thing.");
			}
		}
	}

	//no need for sky box for now
	//drawables.push_back(std::move(std::make_unique<SkyBox>(wnd.Gfx(), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 10000.0f)));
	//drawables.insert(drawables.begin(),std::move(std::make_unique<SkyBox>(wnd.Gfx(), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 10000.0f)));
	bluePlane.SetPos(cam.GetPosition());
	redPlane.SetPos(cam.GetPosition());

	wnd.Gfx().SetProjection(cam.GetProjMatrix());//NearPlaneWidth, NearPlaneHeight, NearZ, FarZ,this will determine FOV angle
	//wall.SetRootTransform(dx::XMMatrixTranslation(-1.5f, 0.0f, 0.0f));
	//tp.SetPos({ 12.0f,0.0f,0.0f });
}

int App::Go()
{
	//if we use GetMessage the getmessage will hang up when there is no message
	//Now we call a function in Window class uses peekmessage which will return 
	//instantly if no message, this will prevent the app from hang up.
	while (true)// call process msg which use peekmsg to not block the loop when no msg.
	{
		
		if (const auto exitcode = Window::ProcessMessages())//singleton static function
		{
			// if return optional has value means we have a Quit message
			return *exitcode;
		}
		
		//Windows window is message (Event) driven, nothing return from processmessage means 
		//there is no message available, And we just keep running the Frame loop.
		DoFrame();
		//this sleep is for you to get control of your window,
		//otherwise the cpu core will be busy doing the loop and you will feel the window is freezing
		//Sleep(1);
	}
}

App::~App()
{}

void App::DoFrame()
{
	auto dt = timer.Mark();
	//std::pair<int, int> DeltaPos;
	//int distX, distY;//will later use int2
	//DeltaPos = wnd.mouse.GetMouseMoveDeltaDist();
	/*
	char msgbuf[1024];
	sprintf_s(msgbuf, "Dist: %d \n",distX);
	OutputDebugString(msgbuf);
	*/
	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	
	//update Cam pos and set cam(currently not working )
	/*
	if (wnd.mouse.RightIsPressed())
	{
		wnd.DisableCursor();
		wnd.mouse.EnableRaw();
		MoveCamera(DeltaPos, dt);
	}
	else if(wnd.mouse.RightIsReleased())
	{
		wnd.EnableCursor();
		wnd.mouse.DisableRaw();
	}*/

	//******Draw Queue*******
	wnd.Gfx().SetCamera(cam.GetMatrix());//set camera 
	cam.Bind(wnd.Gfx());//Bind Camera properties(pos) to PS cbuf slot 3

	//set light this will bind lightingCBUF to the PSCbuf slot0, 
	//if you bind the PSCbuf slot0 in the drawable.cpp, the lighting information will be override.
	light.Bind(wnd.Gfx());

	//(RENDER GEOMETRY)bind a drawables and draw them
	for (auto& d : drawables)
	{
		//d->Update(dt);
		//d->Update(wnd.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt * speed_factor);
		d->Draw(wnd.Gfx());
	}


	//nano.Draw(wnd.Gfx());
	//nano2.Draw(wnd.Gfx());
	//draw light mesh
	//wall.Draw(wnd.Gfx());
	//gobber.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());
	//tp.Draw(wnd.Gfx());
	sponza.Draw(wnd.Gfx());
	bluePlane.Draw(wnd.Gfx());
	redPlane.Draw(wnd.Gfx());
	//******Draw End*******


	/*
	//toggle mouse
	if (wnd.kbd.KeyIsJustPressed(VK_INSERT))
	{
		if (wnd.IsCursorEnabled())
		{
			wnd.DisableCursor();
			wnd.mouse.EnableRaw();
		}
		else
		{
			wnd.EnableCursor();
			wnd.mouse.DisableRaw();
		}
	}*/

	//Check is a key event here
	while (!wnd.kbd.KeyIsEmpty())
	{
		const Keyboard::Event e = wnd.kbd.ReadKey();
		if (e.IsPress() && e.GetCode() == VK_INSERT)
		{
			//OutputDebugString("Insert key is Just pressed!! \n");
		}
	}
	//Check is a mouse event here
	while (!wnd.mouse.QueueIsEmpty())
	{
		const Mouse::Event e = wnd.mouse.Read();
		if (e.GetType() == Mouse::Event::Type::RPress)
		{
			IsMoveingCam = true;
			wnd.DisableCursor();
			wnd.mouse.EnableRaw();
			//OutputDebugString("Insert RMB is Just pressed!! \n");
		}
		if (e.GetType() == Mouse::Event::Type::RRelease)
		{
			IsMoveingCam = false;
			wnd.EnableCursor();
			wnd.mouse.DisableRaw();
			//OutputDebugString("Insert RMB is Just Released!! \n");
		}
	}

	if (IsMoveingCam)
	{
		MoveCamera(dt);
	}

	//*****imgui draw*****
	SpawnSimulationWindow();
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	//SpawnBoxWindowManagerWindow();
	//SpawnBoxWindows();
	ShowImguiDemoWindow();
	//nano.ShowWindow(wnd.Gfx(), "model 1");
	//nano2.ShowWindow("model 2");
	//plane.SpawnControlWindow(wnd.Gfx());
	//wall.ShowWindow(wnd.Gfx(), "Wall");
	//gobber.ShowWindow(wnd.Gfx(), "gobber");
	//tp.SpawnControlWindow(wnd.Gfx());
	sponza.ShowWindow(wnd.Gfx(), "Sponza");
	bluePlane.SpawnControlWindow(wnd.Gfx(), "Blue Plane");
	redPlane.SpawnControlWindow(wnd.Gfx(), "Red Plane");
	//*****imgui end******

	//Flush keyEvent buffer 
	wnd.kbd.FlushKey();
	//present
	wnd.Gfx().EndFrame();
}

void App::MoveCamera(float dt)
{
	int x = 0, y = 0;
	while (const auto d = wnd.mouse.ReadRawDelta())
	{
		x += d->x;
		y += d->y;
	}
	float Forward = 0.0f;
	float Right = 0.0f;
	float Up = 0.0f;

	if (wnd.kbd.KeyIsPressed(0x57) && !wnd.kbd.KeyIsPressed(0x53))
		Forward = 1.0f;
	else if (wnd.kbd.KeyIsPressed(0x53) && !wnd.kbd.KeyIsPressed(0x57))
		Forward = -1.0f;

	if (wnd.kbd.KeyIsPressed(0x44) && !wnd.kbd.KeyIsPressed(0x41))
		Right = 1.0f;
	else if (wnd.kbd.KeyIsPressed(0x41) && !wnd.kbd.KeyIsPressed(0x44))
		Right = -1.0f;

	if (wnd.kbd.KeyIsPressed(VK_SHIFT) && !wnd.kbd.KeyIsPressed(VK_CONTROL))
		Up = 1.0f;
	else if (wnd.kbd.KeyIsPressed(VK_CONTROL) && !wnd.kbd.KeyIsPressed(VK_SHIFT))
		Up = -1.0f;

	//cam.MovePosition(dx::XMFLOAT3(Right, Up, Forward), dt);

	//cam.RotateForward(Dy, Dx, dt); 

	cam.TransformCamera(dx::XMFLOAT3(Right, Up, Forward), float(y), float(x), dt);
}

void App::ShowImguiDemoWindow()
{
	static bool show_demo_window = true;
	if (show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}
}
/*
void App::ShowRawInputWindow()
{
	while (const auto d = wnd.mouse.ReadRawDelta())
	{
		x += d->x;
		y += d->y;
	}
	if (ImGui::Begin("Raw Input"))
	{
		ImGui::Text("Tally:(%d,%d)", x, y);
		ImGui::Text("Cursor : %s", wnd.IsCursorEnabled() ? "enble" : "Disable");
	}
	ImGui::End();
}
*/

void App::SpawnSimulationWindow() noexcept
{
	if (ImGui::Begin("FrameSpeed"))
	{
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();
}

