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



	auto model = Cube::Make(true);

	AddBind(std::make_shared<VertexBuffer>(gfx, model.vertices));

	auto pvs = std::make_shared<VertexShader>(gfx, std::string("SkyBoxVS.cso"));
	auto pvsbc = pvs->GetBytecode();//get ptr to the Blob since the Vertexshader will be deleted
	AddBind(std::move(pvs));

	AddBind(std::make_shared<Texture>(gfx, Surface::FromFile("Images\\EnvMap.png")));
	AddBind(std::make_shared<Sampler>(gfx));

	AddBind(std::make_shared<PixelShader>(gfx, std::wstring(L"SkyBoxPS.cso")));

	AddBind(std::make_shared<IndexBuffer>(gfx, model.indices));

	AddBind(std::make_shared<InputLayout>(gfx, model.vertices.GetLayout().GetD3DLayout(), pvsbc));

	AddBind(std::make_shared<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	
	AddBind(std::make_shared<TransformCbuf>(gfx, *this));

	dx::XMStoreFloat4x4(&mt, dx::XMMatrixTranslation(pos.x, pos.y,pos.z) * dx::XMMatrixScaling(size, size, size));
}

DirectX::XMMATRIX SkyBox::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return  dx::XMLoadFloat4x4(&mt);
}


