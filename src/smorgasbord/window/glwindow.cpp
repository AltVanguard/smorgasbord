#include "glwindow.hpp"

#include <smorgasbord/util/log.hpp>
#include <smorgasbord/util/timer.hpp>

#include <GL/glew.h>

#include <iostream>

using namespace Smorgasbord;

#ifdef WIN32

static void APIENTRY ShowDebugOutput(
	GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar *message, const GLvoid *userParam)
{
	(void)source;
	(void)type;
	(void)id;
	(void)severity;
	(void)length;
	(void)userParam;
	LogI("GL MESSAGE: {0}", message);
}

#else

static void ShowDebugOutput(
	GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar *message, const GLvoid *userParam)
{
	LogI("GL MESSAGE: " + message);
}

#endif

void Smorgasbord::GLWindow::Init(
	int argc, char *argv[], uvec2 windowSize, const string &title)
{
	(void)argc;
	(void)argv;
	this->windowSize = windowSize;

	// Init SDL
	
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LogF("Cannot init SDL");
	}
	
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// minimal depth buffer bits. defaults to 16
	//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	
#if 1 //defined(DEBUG)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
	
	window = SDL_CreateWindow(
			title.c_str(),
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			windowSize.x, windowSize.y,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	
	if (!window)
	{
		string errorMessage = string("SDL Error: ") + SDL_GetError();
		SDL_Quit();
		LogF(errorMessage);
	}
	
	openglContext = SDL_GL_CreateContext(window);
	
	// Init GLEW
	
	// If it's false, some opengl functions cause segmentation fault
	glewExperimental = GL_TRUE;
	
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		LogF("Cannot init GLEW: {0}", (const char*)glewGetErrorString(err));
	}
	
	// Setup GL
	
	glDebugMessageCallback(ShowDebugOutput, NULL);
	LogI((const char*)glGetString(GL_VERSION));
	
	//SDL_GL_SetSwapInterval(1); // TODO: provide setting to enable/disable}
}

void Smorgasbord::GLWindow::CleanupLibs()
{
	SDL_GL_DeleteContext(openglContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Smorgasbord::GLWindow::HandleEvent(SDL_Event windowEvent)
{
	switch (windowEvent.type)
	{
	case SDL_WINDOWEVENT:
		switch (windowEvent.window.event)
		{
		case SDL_WINDOWEVENT_RESIZED:
			windowSize.x = windowEvent.window.data1;
			windowSize.y = windowEvent.window.data2;
			glViewport(0, 0, windowSize.x, windowSize.y);
			return;
		}
		break;
	}
	
	// Handled events should have return'd by now
	Window::HandleEvent(windowEvent);
}
