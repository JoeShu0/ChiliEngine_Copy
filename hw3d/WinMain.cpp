//#include"CustomWin.h"
//#include"WindowsMessageMap.h"
//#include<sstream>

#include"Window.h"
#include"App.h"
//#include<sstream>



int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	try
	{
		return App{}.Go(); //{}means this is a temp variable, but since the message will keep looping, it is OK
		/*
		Window wnd(800, 300, "Box");

		MSG msg;
		BOOL gResult;
		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			while (!wnd.mouse.QueueIsEmpty())
			{
				const auto e = wnd.mouse.Read();
				switch (e.GetType())
				{
				case Mouse::Event::Type::Leave:
					wnd.SetTitle("Gone");
					break;
				case Mouse::Event::Type::Move:
					std::ostringstream oss;
					oss << "Mouse Position: " << e.GetPosX() << " , " << e.GetPosY() << std::endl;
					wnd.SetTitle(oss.str());
				
				}
			}
			
			if (wnd.kbd.KeyIsPressed(VK_SPACE))
			{
				MessageBox(nullptr, "Something Happen", "Space key is pressed", 0);
			}
			if (wnd.kbd.KeyIsPressed(VK_MENU))
			{
				MessageBox(nullptr, "Something Happen", "ALT key is pressed", 0);
			}
			
		}
		//return 0;

		if (gResult == -1)
		{
			return -1;
		}

		return msg.wParam;
		*/
	}
	catch (const CustomException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No detail available", "Unkown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}
