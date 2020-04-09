#pragma once
#include "DrawableBase.h"

class SkyBox : public DrawableBase<SkyBox>
{
public:
	SkyBox(Graphics& gfx, DirectX::XMFLOAT3 pos, float size);
	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT4X4 mt;
};