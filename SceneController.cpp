#include "SceneController.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneResult.h"
#include "Settings.h"
#include "AudioManager.h"

SceneController::SceneController(std::shared_ptr<Settings> settings, std::shared_ptr<AudioManager> audio)
	: m_manager(nullptr)
	, m_settings(settings)
	, m_audio(audio)
{

}

SceneController::~SceneController()
{

}

bool SceneController::SystemInit()
{
	m_manager = std::make_unique<App>();

	// 共有データの設定
	m_manager->get()->settings = m_settings;
	m_manager->get()->audio = m_audio;
	m_manager->get()->isClear = false;

	// シーンを登録
	m_manager->add<SceneTitle>(SceneState::TITLE);
	m_manager->add<SceneGame>(SceneState::GAME);
	m_manager->add<SceneResult>(SceneState::RESULT);

	// 最初のシーンを初期化(フェードインをなくす)
	m_manager->init(SceneState::TITLE, 0s);

	return true;
}

bool SceneController::update()
{
	return m_manager->update();
}
