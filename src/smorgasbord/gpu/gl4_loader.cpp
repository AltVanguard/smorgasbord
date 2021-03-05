#include <smorgasbord/gpu/gl4_loader.hpp>

namespace Smorgasbord {

GL4Loader::GL4Loader(string library_override)
{
	if (library_override.empty())
	{
		library_override = "opengl32.dll";
	}
	
	this->module = LoadLibraryExA(library_override.c_str(), nullptr, 0);
	
	this->wglGetProcAddress = 
		reinterpret_cast<decltype(::wglGetProcAddress)*>(
			GetProcAddress(this->module, "wglGetProcAddress")
			);
	
	LoadProcedures();
}

GL4Loader::~GL4Loader()
{
	FreeLibrary(this->module);
}

void* GL4Loader::LoadProcedure(const char* name)
{
	void* p = reinterpret_cast<void*>(this->wglGetProcAddress(name));
	if (p == nullptr)
	{
		p = reinterpret_cast<void*>(GetProcAddress(this->module, name));
	}
	
	return p;
}

void GL4Loader::LoadProcedures()
{
#define SMORGASBORD_GL_LOAD_PROCEDURE(type, name) \
	this->name \
		= reinterpret_cast<type>(LoadProcedure(#name));
	
	#include <smorgasbord/gpu/gl4_loader_core.inl>
	
#undef SMORGASBORD_GL_LOAD_PROCEDURE
}

}
