#pragma once
#include "Graphics.h"
#include "ConstantBuffers.h"

class Camera
{
public:
	Camera(Graphics& gfx, DirectX::XMMATRIX Proj);

	DirectX::XMMATRIX GetMatrix() const noexcept;
	
	DirectX::XMFLOAT3 GetPosition() const noexcept;
	DirectX::XMFLOAT3 GetForwardVect() const noexcept;

	DirectX::XMMATRIX GetProjMatrix() const noexcept;
	void SetProjectMatrix(DirectX::XMMATRIX newproj) noexcept;

	void Bind(Graphics& gfx) const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	//void MovePosition(DirectX::XMFLOAT3 Dir, float dt) noexcept;
	//void RotateForward(float Pitch, float Yaw, float dt) noexcept;
	void TransformCamera(DirectX::XMFLOAT3 Dir, float Pitch, float Yaw, float dt) noexcept;
	void UpdateCCBufforPS() noexcept;
private:
	struct CameraCBuf
	{
		alignas(16) DirectX::XMFLOAT3 pos;
	} CCBuf;
private:
	DirectX::XMMATRIX ProjectionMAT;
	DirectX::XMFLOAT3 Pos = DirectX::XMFLOAT3(0.0f,0.0f,-20.0f);
	DirectX::XMFLOAT3 Forward = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	float Roll = 0;//rad
	float FlySpeed = 10.0f;//unit per second
	float PanFactor = 0.0045f;//rad per pixel
	mutable Bind::PixelConstantBuffer<CameraCBuf> cbuf;
};