#pragma once
#include "Bindable.h"

class Surface;

namespace Bind
{
	class Texture : public Bindable
	{
	public:
		Texture(Graphics& gfx, std::string& path , UINT slot = 0);
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<Texture> Resolve(Graphics& gfx, std::string& path, UINT slot = 0);
		static std::string GenerateUID(const std::string& path, UINT slot = 0);
		std::string GetUID() const noexcept override;
	protected:
		std::string path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	private:
		unsigned int slot;
	};
}