#pragma once
#include "CustomWin.h"
#include "CustomException.h"
#include <d3d11.h>
#include <vector>
#include <wrl.h>
#include "DxgiInfoManager.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>
#include "ConditionalNoexcept.h"

namespace Bind
{
	class Bindable;
}

class Graphics 
{
	friend Bind::Bindable;
public:
	class Exception : public CustomException
	{
		using CustomException::CustomException;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {} )noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
public:
	Graphics(HWND hWnd, int width, int hight);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	void EndFrame(); //Swap buffer
	void BeginFrame(float red, float green, float blue)noexcept;
	//void DrawTestTriangle(float Offset, float x, float y);
	void DrawIndexed(UINT count) noxnd;
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetCamera(DirectX::XMMATRIX cam)noexcept;
	DirectX::XMMATRIX GetCamera() noexcept;
	//imgui stuff
	void EnableImgui() noexcept;
	void DisableImgui() noexcept;
	bool IsImguiEnable() const noexcept;
private:
	bool imguiEnable = true;
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX Camera;
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif // NDEBUG
	
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
	/*
	std::unique_ptr<ID3D11Device> pDevice;
	std::unique_ptr<IDXGISwapChain> pSwap;
	std::unique_ptr<ID3D11DeviceContext> pContext;
	std::unique_ptr<ID3D11RenderTargetView> pTarget;
	*/
};