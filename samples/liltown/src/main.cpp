#include "widget/mainwidget.hpp"

#include <smorgasbord/rendering/scenegraph.hpp>
#include <smorgasbord/util/log.hpp>
#include <smorgasbord/window/glwindow.hpp>

#include <fmt/format.h>

#include <iostream>
#include <functional>

SMORGASBORD_SET_LOG(std::cout, LogAssertLevel::Error);

using namespace std;
using namespace glm;
using namespace Smorgasbord;

int main(int argc, char *argv[])
{
	uvec2 windowSize = uvec2(1280, 720);
	shared_ptr<MainWidget> mainWidget = make_shared<MainWidget>(windowSize);
	
	shared_ptr<GLWindow> mainWindow = make_shared<GLWindow>();
	mainWindow->Init(argc, argv, windowSize, "Lil Town");
	mainWidget->Setup(mainWindow, mainWindow->GetGL4Device());
	mainWindow->EnterMainLoop(mainWidget);
	
	return 0;
}
