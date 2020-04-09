#include "VertexShader.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	VertexShader::VertexShader(Graphics & gfx, std::wstring & path)
	{
		INFOMAN(gfx);

		GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &pBytecodeBlob));
		GFX_THROW_INFO(GetDevice(gfx)->CreateVertexShader(
										pBytecodeBlob->GetBufferPointer(), 
										pBytecodeBlob->GetBufferSize(), 
										nullptr, 
										&pVertexShader));
	}

	void VertexShader::Bind(Graphics & gfx) noexcept
	{
		GetContext(gfx)->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	}

	ID3DBlob * VertexShader::GetBytecode() const noexcept
	{
		return pBytecodeBlob.Get();
		//this will the pointer to the Blob on DX side
		//Will incease reference.
	}
}
