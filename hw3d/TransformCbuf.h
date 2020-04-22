#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>

//this is a special class, we want this buffer to be the same with the VerTexConstantBuffer,
//But with a ptr to the drawable object.Since we failed to get the Ctor to work for additional ptr
//we will include a VertexConstantBuffer into our class, so we can construct it in our Ctor
namespace Bind
{
	class TransformCbuf : public Bindable
	{
	private:
		struct Transforms
		{
			DirectX::XMMATRIX Model; // model local to world
			DirectX::XMMATRIX ModelView; // model local to view local
			DirectX::XMMATRIX ModelViewProj; // model local to screen
		};
	public:
		TransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot = 0u);
		void Bind(Graphics& gfx) noexcept override;
	private:
		//declare a instance to VertexConstanBuffer class (with matrix), Construct it in the Ctor
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
		const Drawable& parent;
	};
}

