#include "SkyBox.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Cube.h"
#include "Surface.h"
#include "Texture.h"
#include "Sampler.h"

using namespace Bind;

SkyBox::SkyBox(Graphics & gfx, DirectX::XMFLOAT3 pos, float size)
{
	namespace dx = DirectX;

	struct Vertex
	{
		dx::XMFLOAT3 pos;
	};

	auto model = Cube::Make<Vertex>(true);

	AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

	auto pvs = std::make_unique<VertexShader>(gfx, std::wstring(L"SkyBoxVS.cso"));
	auto pvsbc = pvs->GetBytecode();//get ptr to the Blob since the Vertexshader will be deleted
	AddStaticBind(std::move(pvs));

	AddStaticBind(std::make_unique<Texture>(gfx, Surface::FromFile("Images\\EnvMap.png")));
	AddStaticBind(std::make_unique<Sampler>(gfx));

	AddStaticBind(std::make_unique<PixelShader>(gfx, std::wstring(L"SkyBoxPS.cso")));

	AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

	AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	
	AddBind(std::make_unique<TransformCbuf>(gfx, *this));

	dx::XMStoreFloat4x4(&mt, dx::XMMatrixTranslation(pos.x, pos.y,pos.z) * dx::XMMatrixScaling(size, size, size));
}

void SkyBox::Update(float dt) noexcept
{

}

DirectX::XMMATRIX SkyBox::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return  dx::XMLoadFloat4x4(&mt);
}


