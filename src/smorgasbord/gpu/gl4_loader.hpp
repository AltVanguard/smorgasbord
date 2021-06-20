#pragma once

#include <GL/glcorearb.h>

#include <GL/wglext.h>

#include <string>

// TODO: guard dll loading with WIN32

namespace Smorgasbord {

class GL4Loader {
private:
	HMODULE module;
	decltype(::wglGetProcAddress)* wglGetProcAddress = nullptr;

public:
#define SMORGASBORD_GL_LOAD_PROCEDURE(type, name) type name = nullptr;

#include <smorgasbord/gpu/gl4_loader_core.inl>

#undef SMORGASBORD_GL_LOAD_PROCEDURE

public:
	GL4Loader(std::string library_override = "");
	~GL4Loader();
	
	void SetIsEnabled(GLenum key, bool value) const
	{
		if (value)
		{
			glEnable(key);
		}
		else
		{
			glDisable(key);
		}
	}
	
private:
	void* LoadProcedure(const char* name);
	void LoadProcedures();
};

}
