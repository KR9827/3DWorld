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
struct SceneData
{

};

using App = SceneManager<SceneState, SceneData>;

class SceneController
{
public:
	SceneController();
	~SceneController();

	bool SystemInit();
	bool update();

private:
	std::unique_ptr<App> m_manager;

};
