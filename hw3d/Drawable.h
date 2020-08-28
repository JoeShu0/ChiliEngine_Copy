#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"
#include <memory>

namespace Bind
{
	class Bindable;
	class IndexBuffer;
}

class Drawable
{
	//template<class T>
	//friend class DrawableBase;
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noxnd;
	
	virtual ~Drawable() = default;

	template<class T>
	T* QueryBindable() noexcept
	{
		for (auto& pb : binds)
		{
			if (auto pt = dynamic_cast<T*>(pb.get()))
			{
				return pt;
			}
		}
		return nullptr;
	}
protected:
	void AddBind(std::shared_ptr<Bind::Bindable> bind) noxnd;
	//void AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer> ibuf) noxnd;
//private:
	//we maintain a ptr to the IndexBuffer so we can get the index count when drawing.
	//const IndexBuffer* pIndexBuffer = nullptr;

	//virtual const std::vector<std::unique_ptr<Bind::Bindable>>& GetStaticBinds() const noexcept = 0;
private:
	const Bind::IndexBuffer* pIndexBuffer = nullptr;
	// Maintain a list of unique_ptrs to all the bindable resources for later BINDing.
	std::vector<std::shared_ptr<Bind::Bindable>> binds;
};