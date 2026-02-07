#include "Application.h"
#include "SceneController.h"

Application::Application()
	: m_scene(nullptr)
{

}

Application::~Application()
{

}

bool Application::SystemInit()
{
	Window::Resize(WINDOW_WIDTH, WINDOW_HEIGHT);
	Window::SetTitle(U"3D World");

	m_scene = std::make_unique<SceneController>();
	if (m_scene == nullptr) return false;

	if (!m_scene->SystemInit()) return false;

	return true;
}

void Application::Run()
{
	while (System::Update())
	{
		if (!m_scene->update()) break;
	}
}
