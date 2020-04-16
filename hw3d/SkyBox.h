#pragma once
#include "DrawableBase.h"

class SkyBox : public Drawable
{
public:
	SkyBox(Graphics& gfx, DirectX::XMFLOAT3 pos, float size);
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT4X4 mt;
};