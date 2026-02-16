#pragma once

class Application
{
public:
	Application();
	~Application();

	bool SystemInit();
	void Run();

private:
	static constexpr int32 WINDOW_WIDTH{ 1280 }, WINDOW_HEIGHT{ 720 };

	std::shared_ptr<class Settings> m_settings;
	std::shared_ptr<class AudioManager> m_audio;
	std::unique_ptr<class SceneController> m_scene;
};
