#pragma once
#include "Graphics.h"
#include "ConditionalNoexcept.h"

// this is the base class for all DX resources that canbe binded into the pipeline.
// every child class have to have a Bind function.

namespace Bind
{
	class Bindable
	{
	public:
		virtual void Bind(Graphics& gfx) noexcept = 0;
		virtual ~Bindable() = default; //polymophism need implement custom Dtor??
	protected:
		// Since all creating and binding DX resources need interface to Context and Device(private to the Graohics)
		// We declare friend from Graphics to Bindable, And we can have 2 getter Func to get the private ptr for Context and Device.
		// Now every child can use them to get the interface needed. (Friend relation do not inherent)
		static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
		static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
		//Some as above but the INFOmanager is only available in debug
		static DxgiInfoManager& GetInfoManager(Graphics& gfx) ;
	};
}