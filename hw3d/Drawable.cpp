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
	for (auto& b : GetStaticBinds())
	{
		b->Bind(gfx);
	}
	gfx.DrawIndexed(pIndexBuffer->GetCount());//here we need the Ptr to indexbuffer for index count
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noxnd
{
	assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	binds.push_back(std::move(bind));
	//We construct the Binadable unique ptr and move to transfer own ship into this list
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noxnd
{
	assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
	//the same as above and we only use this to bind the indexBuffer so we can get the ptr of the indexbuffer in memory
	pIndexBuffer = ibuf.get();
	binds.push_back(std::move(ibuf));
}
