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

	std::unique_ptr<class SceneController> m_scene;
};
