#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	template<typename C>
	class ConstantBuffer : public Bindable
	{
	public:
		//All constant buffers should have a update function Since you are going to updateit when needed. 
		void Update(Graphics& gfx, const C& consts)
		{
			INFOMAN(gfx);
			//this will create HRESULT hr, also get the info manager in debug mode
			D3D11_MAPPED_SUBRESOURCE msr;
			//Context.Map will lock the resource and get a ptr to the memory
			GFX_THROW_INFO(GetContext(gfx)->Map(
				pConstantBuffer.Get(), 0u,
				D3D11_MAP_WRITE_DISCARD, 0u, &msr
			));
			//memcpy Copy direct into the memory(Binary Copy)
			memcpy(msr.pData, &consts, sizeof(consts));
			//Context.Unmap to unlock the memory
			GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
			//map the buffer and modify its content and unmap
		}

		//Ctor with initial Data.
		ConstantBuffer(Graphics& gfx, const C& consts, UINT slot = 0u)
			:
			slot(slot)
		{
			INFOMAN(gfx);

			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;//being updated every frame.
			cbd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(consts);
			cbd.StructureByteStride = 0u;

			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = &consts;
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstantBuffer));
		}

		//Ctor without initial data
		ConstantBuffer(Graphics& gfx, UINT slot = 0u)
			:
			slot(slot)
		{
			INFOMAN(gfx);

			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(C);
			cbd.StructureByteStride = 0u;
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
		}

	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		UINT slot = 0;
	};


	//When nherenting from template classes and you want to access the protected member.
	//you have to :using class<C>::ProtectedMember or this->ProtectedMember
	template<typename C>
	class VertexConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer;
		using ConstantBuffer<C>::slot;
		using Bindable::GetContext;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void Bind(Graphics& gfx) noexcept override
		{
			GetContext(gfx)->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		}
		static std::shared_ptr<Bindable> Resolve(Graphics &gfx)
		{
			return Codex::Resolve<VertexConstantBuffer>(gfx);
		}
		static std::string GenerateUID()
		{
			return typeid(VertexConstantBuffer).name();
		}
		std::string GetUID() const noexcept override
		{
			return GenerateUID();
		}
	};

	template<typename C>
	class PixelConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::pConstantBuffer;
		using ConstantBuffer<C>::slot;
		using Bindable::GetContext;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void Bind(Graphics& gfx) noexcept override
		{
			GetContext(gfx)->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		}
		std::shared_ptr<Bindable> Resolve(Graphics& gfx)
		{
			return Codex::Resolve<PixelConstantBuffer>(gfx);
		}
		static std::string GenerateUID()
		{
			return typeid(PixelConstantBuffer).name();
		}
		std::string GetUID() const noexcept override
		{
			return GenerateUID();
		}
	};

}