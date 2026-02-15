#include "SceneController.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneResult.h"

SceneController::SceneController()
	: m_manager(nullptr)
{

}

SceneController::~SceneController()
{

}

bool SceneController::SystemInit()
{
	m_manager = std::make_unique<App>();

	// 共有データの設定
	m_manager->get()->isClear = false;

	// シーンを登録
	m_manager->add<SceneTitle>(SceneState::TITLE);
	m_manager->add<SceneGame>(SceneState::GAME);
	m_manager->add<SceneResult>(SceneState::RESULT);

	// 最初のシーンを初期化(フェードインをなくす)
	m_manager->init(SceneState::GAME, 0s);

	return true;
}

bool SceneController::update()
{
	return m_manager->update();
}
