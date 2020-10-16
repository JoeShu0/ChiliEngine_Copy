#include "Camera.h"
#include "imgui/imgui.h"

namespace dx = DirectX;


Camera::Camera(Graphics & gfx, DirectX::XMMATRIX Proj)
	:
	ProjectionMAT(Proj),
	cbuf(gfx, 3)//Set the cameraCbuf to be bind to slot3!!! as the slot is shared, caution when adding slot to otherbindings!!
{
	Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	 //auto pos = dx::XMLoadFloat3(Pos);
	/*
	const auto pos = dx::XMVector3Transform(
		dx::XMVectorSet(0.0f, 0.0f, -r, 0.0f),
		dx::XMMatrixRotationRollPitchYaw(phi, -theta, 0.0f)
	);
	
	return dx::XMMatrixLookAtLH(
		pos, dx::XMVectorZero(),
		dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)) * dx::XMMatrixRotationRollPitchYaw(pitch, -yaw, roll);
		*/

	//*a = dx::XMFLOAT3(0.0f, 0.0f, 1.0f);
	//dx::XMStoreFloat3(&Forward,dx::XMVector3Normalize(dx::XMLoadFloat3(&Forward)));
	//
	return dx::XMMatrixLookToLH(
			dx::XMLoadFloat3(&Pos), 
			dx::XMLoadFloat3(&Forward), 
			dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)) * dx::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, Roll);
}

DirectX::XMFLOAT3 Camera::GetPosition() const noexcept
{
	return Pos;
}

DirectX::XMFLOAT3 Camera::GetForwardVect() const noexcept
{
	return Forward;
}

DirectX::XMMATRIX Camera::GetProjMatrix() const noexcept
{
	return ProjectionMAT;
}

void Camera::SetProjectMatrix(DirectX::XMMATRIX newproj) noexcept
{
	ProjectionMAT = newproj;
}

void Camera::Bind(Graphics & gfx) const noexcept
{
	cbuf.Update(gfx, CCBuf);
	cbuf.Bind(gfx);
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &Pos.x, -100.0f, 100.0f, "%.1f");
		ImGui::SliderFloat("y", &Pos.y, -100.0f, 100.0f, "%.1f");
		ImGui::SliderFloat("z", &Pos.z, -100.0f, 100.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderFloat("Fx", &Forward.x, -1.0f, 1.0f, "%.5f");
		ImGui::SliderFloat("Fy", &Forward.y, -1.0f, 1.0f, "%.5f");
		ImGui::SliderFloat("Fz", &Forward.z, -1.0f, 1.0f, "%.5f");
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	Pos = dx::XMFLOAT3(0.0f, 0.0f, -20.0f);
	Forward = dx::XMFLOAT3(0.0f, 0.0f, 20.0f);
	Roll = 0.0f;
}

void Camera::TransformCamera(DirectX::XMFLOAT3 Dir, float Pitch, float Yaw, float dt) noexcept
{
	using namespace DirectX;
	const auto Upvector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const auto ForwardVector = XMVector3Normalize(XMLoadFloat3(&Forward));
	const auto CameraRight = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&Forward), Upvector));

	const auto NewPos = dx::XMLoadFloat3(&Pos)
		+ FlySpeed * dt * (
			Dir.z * ForwardVector + Dir.y * Upvector + -Dir.x * CameraRight);
	dx::XMStoreFloat3(&Pos, NewPos);

	const auto RotationMatrix = XMMatrixRotationNormal(CameraRight, -Pitch * PanFactor) * XMMatrixRotationNormal(Upvector, Yaw * PanFactor);
	const auto NewForwad = XMVector3TransformNormal(ForwardVector, RotationMatrix);
	XMStoreFloat3(&Forward, NewForwad);

	UpdateCCBufforPS();
}

void Camera::UpdateCCBufforPS() noexcept
{
	CCBuf.pos = Pos;
}

