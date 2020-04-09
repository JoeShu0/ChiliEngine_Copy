#include "TransformCbuf.h"

namespace Bind
{
	std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVcbuf;

	TransformCbuf::TransformCbuf(Graphics & gfx, const Drawable & parent, UINT slot)
		:
		parent(parent)
	{
		if (!pVcbuf)
		{
			pVcbuf = std::make_unique<VertexConstantBuffer<TransformCbuf::Transforms>>(gfx, slot);
		}
	}

	void TransformCbuf::Bind(Graphics & gfx) noexcept
	{
		const auto model = parent.GetTransformXM();
		const TransformCbuf::Transforms tf = {
			DirectX::XMMatrixTranspose(model),
			DirectX::XMMatrixTranspose(model * gfx.GetCamera()),
			DirectX::XMMatrixTranspose(model * gfx.GetCamera() * gfx.GetProjection()) // use XMMatrixTranspose to stack transform matrix.
		};
		//the transpose here is for matrix row column major difference btween CPU and GPU.
		pVcbuf->Update(gfx, tf);
		pVcbuf->Bind(gfx);
	}
}

