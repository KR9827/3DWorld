#pragma once
#include <Siv3D.hpp>

// シーンの状態管理
enum class SceneState
{
	TITLE,
	GAME,
	RESULT,
};

// シーン間で共有するデータ
struct ShareData
{
	std::shared_ptr<class Settings> settings;
	std::shared_ptr<class AudioManager> audio;
	bool isClear;
};

using App = SceneManager<SceneState, ShareData>;

class SceneController
{
public:
	SceneController(std::shared_ptr<class Settings> settings, std::shared_ptr<class AudioManager> audio);
	~SceneController();

	bool SystemInit();
	bool update();

private:
	std::unique_ptr<App> m_manager;
	std::shared_ptr<class Settings> m_settings;
	std::shared_ptr<class AudioManager> m_audio;
};
