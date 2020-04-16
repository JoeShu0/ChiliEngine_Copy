#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>
//#include <typeinfo>

using namespace Bind;

void Drawable::Draw(Graphics & gfx) const noxnd
{
	//When drawing this object, we are going to bind all the component (every frame)
	for (auto& b : binds)
	{
		b->Bind(gfx);
	}
	gfx.DrawIndexed(pIndexBuffer->GetCount());//here we need the Ptr to indexbuffer for index count
}

void Drawable::AddBind(std::shared_ptr<Bindable> bind) noxnd
{
	//special cases for index buffer
	if (typeid(*bind) == typeid(Bind::IndexBuffer))
	{
		assert("Binding multiple index buffers not allowed" && pIndexBuffer == nullptr);
		// de-reference for the bind shared_ptr, cast into reference of IndexBuffer, get its address assign to the pIndexBuffer
		pIndexBuffer = &static_cast<IndexBuffer&>(*bind);
	}
	binds.push_back(std::move(bind));
	//We construct the Binadable unique ptr and move to transfer own ship into this list
}


