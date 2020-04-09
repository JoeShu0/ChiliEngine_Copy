#include "Box.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Cube.h"

#include "imgui/imgui.h"

Box::Box(Graphics & gfx, std::mt19937 & rng,
	std::uniform_real_distribution<float>& adist, 
	std::uniform_real_distribution<float>& ddist, 
	std::uniform_real_distribution<float>& odist, 
	std::uniform_real_distribution<float>& rdist, 
	std::uniform_real_distribution<float>& bdist,
	DirectX::XMFLOAT3 material)
	:
	/*
	r(rdist(rng)),
	droll(ddist(rng)),
	dpitch(ddist(rng)),
	dyaw(ddist(rng)),
	dphi(odist(rng)),
	dtheta(odist(rng)),
	dchi(odist(rng)),
	chi(adist(rng)),
	theta(adist(rng)),
	phi(adist(rng))*/
	TestObject(gfx,rng,adist,ddist,odist,rdist)
{
	namespace dx = DirectX;

	if (!IsStaticInitialized())
	{
		struct Vertex
		{
			dx::XMFLOAT3 pos;
			dx::XMFLOAT3 n;
		};

		auto model = Cube::MakeIndependent<Vertex>();
		model.SetNormalsIndependentFlat();

		//model.Transform(dx::XMMatrixScaling(1.0f, 1.0f, 0.25f));

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

		auto pvs = std::make_unique<VertexShader>(gfx, std::wstring(L"PhongVS.cso"));
		auto pvsbc = pvs->GetBytecode();//get ptr to the Blob since the Vertexshader will be deleted
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, std::wstring(L"PhongPS.cso")));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));
		/*
		struct PSLightConstants
		{
			dx::XMVECTOR pos;
		};
		//this just bind a pcbuf of a struct with a vector?????
		//the buffer will be only updated by PointLight??
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSLightConstants>>(gfx));
		*/
		

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
		};
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	}
	else
	{
		//when the static binds are all initialized, we still need to fill the indexptr in every instance. 
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
	
	materialConstants.color = material;
	AddBind(std::make_unique<MaterialCbuf>(gfx, materialConstants, 1u));

	//store model deform info into the float33
	dx::XMStoreFloat3x3(&mt, dx::XMMatrixScaling(1.0f, 1.0f, bdist(rng)));

}
/*
void Box::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}*/

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	//return dx::XMLoadFloat3x3(&mt) *
		//dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		//dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
		//dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
	return dx::XMLoadFloat3x3(&mt) * TestObject::GetTransformXM();
}

bool Box::SpawnControlWindow(int id, Graphics& gfx) noexcept
{
	using namespace std::string_literals;

	bool dirty = false;
	bool open = true;
	if (ImGui::Begin(("Box "s + std::to_string(id)).c_str(), &open))
		//&open will make the window show a X widget, close the window and the &open will be set to false
	{
		const auto cd = ImGui::ColorEdit3("Material Color", &materialConstants.color.x);
		const auto sid = ImGui::SliderFloat("Specular Intensity", &materialConstants.specularIntensity, 0.05f, 4.0f, "%.2f", 2);
		const auto spd = ImGui::SliderFloat("Specular Power", &materialConstants.specularPower, 1.0f, 200.0f, "%.2f", 2);
		dirty = cd || sid || spd;

		ImGui::Text("Position");
		ImGui::SliderFloat("R", &r, 0.0f, 80.0f, "%.1f");
		ImGui::SliderAngle("Theta", &theta, -180.0f, 180.0f);
		ImGui::SliderAngle("Phi", &phi, -180.0f, 180.0f);
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
	}
	ImGui::End();

	if (dirty)
	{
		SyncMaterial(gfx);
	}
	return open;
}

void Box::SyncMaterial(Graphics& gfx) noexcept(!IS_DEBUG)
{
	auto pConstPS = QueryBindable<MaterialCbuf>();
	assert(pConstPS != nullptr);
	pConstPS->Update(gfx, materialConstants);
}
