#include "Application.h"
#include "SceneController.h"
#include "Settings.h"
#include "AudioManager.h"

Application::Application()
	: m_scene(nullptr)
	, m_settings(nullptr)
	, m_audio(nullptr)
{

}

Application::~Application()
{

}

bool Application::SystemInit()
{
	Window::Resize(WINDOW_WIDTH, WINDOW_HEIGHT);
	Window::SetTitle(U"3D World");

	m_settings = std::make_shared<Settings>();
	if (m_settings == nullptr) return false;
	m_audio = std::make_shared<AudioManager>(m_settings);
	if (m_audio == nullptr) return false;
	m_scene = std::make_unique<SceneController>(m_settings, m_audio);
	if (m_scene == nullptr) return false;

	if (!m_settings->Load()) return false;
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
