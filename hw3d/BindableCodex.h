#pragma once

#include "Bindable.h"
#include "BindableCodex.h"
#include <type_traits>
#include <memory>
#include <unordered_map>

namespace Bind
{
	//we make the codex a singleton 
	class Codex 
	{
	public:
		//Here we will bring in all th param
		template<class T, typename...Params>
		static std::shared_ptr<T> Resolve(Graphics& gfx, Params&&...p) noxnd
		{
			static_assert(std::is_base_of<Bindable, T>::value, "Can only resolve classes derived from Bindable");
			return Get().Resolve_<T>(gfx, std::forward<Params>(p)...);
		}
	private:
		template<class T, typename...Params>
		std::shared_ptr<T> Resolve_(Graphics &gfx, Params&&...p) noxnd
		{
			//Every shared bindable class have to have a static GenerateUID func
			//since in order to generate UID, we need all the Ctor params
			//We will get all the params in Resolve() and forward to GenerrateUID
			const auto key = T::GenerateUID(std::forward < Params>(p)...);
			const auto i = binds.find(key);
			if (i == binds.end())
			{
				auto bind = std::make_shared<T>(gfx, std::forward<Params>(p)...);
				binds[key] = bind;
				return bind;
			}
			else
			{
				return std::static_pointer_cast<T>(i->second);
				//return dynamic_cast<T*>(i->second.get()); //will this work??
			}
		}
		static Codex& Get() //static get so we will only get 1 instance for this class
		{
			static Codex codex;
			return codex;
		}
	private:
		//we use an unorederd map(HASH map) to map the string identification to actuall bindables
		//so we can reuse the bindable as we see the same identification string
		std::unordered_map<std::string, std::shared_ptr<Bindable>> binds;
	};
}