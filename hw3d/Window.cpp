#include "Window.h"
#include <sstream>
#include "resource.h"
#include "WindowsThrowMacros.h"
#include "imgui/imgui_impl_win32.h"
//#include "WindowsMessageMap.h"

Window::WindowClass Window::WindowClass::wndClass;//allocate space for the static instance wndClass


Window::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32,32,0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

Window::Window(int width, int height, const char * name)
	:
	width(width),
	height(height)
{
	//Cal the Actual Window Size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw CUWND_LAST_EXCEPT();
	}
	//create window
	hWnd = CreateWindow(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this); // the final this pointer is passed in to store a ptr to this class in WIN32 side?
	if (hWnd == nullptr)
	{
		throw CUWND_LAST_EXCEPT();
	}
	//show window
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	//Init ImGui Win32 Impl
	ImGui_ImplWin32_Init(hWnd);
	//throw CUWND_EXCEPT(ERROR_BAD_FORMAT);//Test error code
	//Create Graphics objects
	pGfx = std::make_unique<Graphics>(hWnd, width, height);
	//We are using a unique ptr here to create a Graphics object
	//Since the creation of Graphics pobject need hWnd, we have to deffer it
	
	//register mouse raw input device
	//otherwise windows will not sent raw message to this program
	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01;//mouse page
	rid.usUsage = 0x02;//mouse usage
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;
	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
	{
		throw CUWND_LAST_EXCEPT();
	}
}

Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hWnd);
}


void Window::SetTitle(const std::string & title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw CUWND_LAST_EXCEPT();
	}
}

void Window::EnableCursor() noexcept
{
	cursorEnable = true;
	ShowCursor();
	EnableImGuiMouse();
	FreeCursor();
}

void Window::DisableCursor() noexcept
{
	cursorEnable = false;
	HideCursor();
	DisableImGuiMouse();
	ConfineCursor();
}

bool Window::IsCursorEnabled() const noexcept
{
	return cursorEnable;
}

std::optional<int> Window::ProcessMessages() noexcept
{
	MSG msg;
	//While queue has message, remove and dispatch them(do not block)
	//the return value of this peekmessage only says if there is any messages.
	//the IMPORTANT STUFF there is that we now use peekmessage instead of getmessage
	//when no msg, Peek will return instantly, Get will hang up and wait for message
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		//check for quit because pekmessage does not signal this via return
		if (msg.message == WM_QUIT)
		{
			//return optional wrapping int 
			return msg.wParam;
		}
		//only if there is message and message is not quit, we are going to do
		//trans and disp msg, this will activate the app
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//return empty optional when no messages available, let the app know there is no msg
	//And  do frame loop to keep the app running.
	return {}; 
}

Graphics & Window::Gfx()
{
	if (!pGfx)
	{
		throw CUWND_NOGFX_EXCEPT();
	}
	return *pGfx;
}
void Window::ConfineCursor() noexcept
{
	RECT rect;
	//Get the size of the window
	GetClientRect(hWnd, &rect);
	//Convert the window boarder to screen space
	MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
	//limit the mouse with in the window.
	ClipCursor(&rect);
}

void Window::FreeCursor() noexcept
{
	ClipCursor(nullptr);
}


//the ShowCursor return value is a incremental value 
//each time it is called the value will increment
//inorder to get the cursor to hide or show, we need to loop all the way to make it flip the sign
void Window::HideCursor() noexcept
{
	while(::ShowCursor(FALSE) >= 0);
}

void Window::ShowCursor() noexcept
{
	while (::ShowCursor(TRUE) <= 0);
}

void Window::EnableImGuiMouse() noexcept
{
	ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void Window::DisableImGuiMouse() noexcept
{
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	//return DefWindowProc(hWnd, msg, wParam, lParam);
	// use create parameter passed in from CreateWindow() to store window class ptr
	if (msg == WM_NCCREATE)
	{
		//extact ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		//set WinAPI managed user data to store the window class ptr
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		//set messags proc to normal (non-setup) handler now that the setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	// if we get message before the WM_NCCREATE, handle it with default
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	//retrieve ptr to window class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	//forward message to window class handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//imGui stuff
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}
	const auto& imio = ImGui::GetIO();
	
	switch (msg)
	{
	//******Close window******
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;//returns here not using thr defWindowProc to destroy the window, we use the destructor.
	//******Mouse Stuff******
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		//cursorless exclusive get the first dibs
		if (!cursorEnable)//block all mouseMove MSG when the cursor is hidden,So mouse will not get any MouseMove MSG
		{
			if (!mouse.IsInWindow())
			{
				//since this message will only appears when the cursor is on the window.
				//Hide the cursor as soon as the cursor enters the window from outside.
				SetCapture(hWnd);
				mouse.OnMouseEnter();
				HideCursor();
			}
			break;
		}
		
		if (imio.WantCaptureMouse)
			break;//mouse msg captured by imgui

		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
		{
			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		else
		{
			if (mouse.LeftIsPressed() || mouse.RightIsPressed() || mouse.WheeelIsPressed())
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		
		break;
	}
	case WM_LBUTTONDOWN:
	{
		SetForegroundWindow(hWnd);

		if (!cursorEnable)
		{
			//OutputDebugString("click => recapture \n");
			ConfineCursor();
			HideCursor();
		}//this will make sure to capture and confine mouse once the client rect is clicked

		if (imio.WantCaptureMouse)
			break;//mouse msg captured by imgui

		mouse.OnLeftPressed();
		break;
	}
	case WM_LBUTTONUP:
	{
		SetForegroundWindow(hWnd);
		
		if (imio.WantCaptureMouse)
			break;//mouse msg captured by imgui

		mouse.OnLeftReleased();
		break;
	}
	case WM_RBUTTONDOWN:
	{
		if (imio.WantCaptureMouse)
			break;//mouse msg captured by imgui

		mouse.OnRightPressed();
		break;
	}
	case WM_RBUTTONUP:
	{
		if (imio.WantCaptureMouse)
			break;//mouse msg captured by imgui
		
		mouse.OnRightReleased();
		break;
	}
	case WM_MBUTTONDOWN:
	{
		if (imio.WantCaptureMouse)
			break;//mouse msg captured by imgui
		
		mouse.OnWheelPressed();
		break;
	}
	case WM_MBUTTONUP:
	{
		if (imio.WantCaptureMouse)
			break;//mouse msg captured by imgui
		
		mouse.OnWheelReleased();
		break;
	}
	case WM_MOUSEWHEEL:
	{
		if (imio.WantCaptureMouse)
			break;//mouse msg captured by imgui
		
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(delta);
		break;
	}

	//Clear keystate when not focused on,  to avoid key stay pressed when switch to other window.
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;
	case WM_ACTIVATE:
		//OutputDebugString("Activate\n");
		if (!cursorEnable)
		{
			if (wParam & WA_ACTIVE || wParam & WA_CLICKACTIVE)
			{
				//OutputDebugString("Active => Confine \n");
				ConfineCursor();
				HideCursor();
			}
			else
			{
				//OutputDebugString("Active => Free \n");
				FreeCursor();
				ShowCursor();
			}
		}
	//******Keyboard stuff******
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: //syskey commands need to be handled to track ALT key
		if (imio.WantCaptureKeyboard)
			break;//msg being captured by imgui
		if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnable()) //the 30 digit of lParam is 1 if the key is repeated.
		{
			//OutputDebugString("OnkeyPressed is called!! \n");
			kbd.OnKeyPressed(static_cast<char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (imio.WantCaptureKeyboard)
			break;//msg being captured by imgui

		//OutputDebugString("OnkeyReleased is called!! \n");
		kbd.OnKeyReleased(static_cast<char>(wParam));
		break;
	case WM_CHAR:
		if (imio.WantCaptureKeyboard)
			break;//msg being captured by imgui

		kbd.OnChar(static_cast<char>(wParam));
		break;
	
	//****** Raw Mouse Messsge *******
	case WM_INPUT:
	{
		if (!mouse.IsRawEnabled())
		{ break; }
		
		UINT size;
		//Since Get the raw data need to know the size of the data
		//first get the size of the input data
		if(GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			nullptr,
			&size,
			sizeof(RAWINPUTHEADER)) == -1)
		{
			//bail msg processing if error
			break;
		}
		rawBuffer.resize(size);
		//read in the input data
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			rawBuffer.data(),
			&size,
			sizeof(RAWINPUTHEADER)) != size)
		{
			//bail msg processing if error
			break;
		}
		//process the raw input data
		auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
		if (ri.header.dwType == RIM_TYPEMOUSE &&
			(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
		{
			mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
		}
		break;
	}
	//****** exception Test ******
	/*
	case WM_LBUTTONDOWN:
		if (wParam & MK_CONTROL)
			throw CUWND_EXCEPT(ERROR_BAD_FORMAT);//Test error code, this error will be cautch in windows
			*/
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
// << Window Exception stuff
Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	hr(hr)
{}

const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
	return "Custom Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	const DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr);
		// when use the FORMAT_MESSAGE_ALLOCATE_BUFFER, you should supply a ptr to ptr(the adress of that ptr),it will make that ptr pt to the error string buffer.
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exception::TranslateErrorCode(hr);
}

const char * Window::NoGfxException::GetType() const noexcept
{
	return "Custom Window Exception [No Graphics]";
}

//Window exception Stuff >>
