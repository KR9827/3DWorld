#pragma once
#include "OptionBase.h"
#include "Settings.h"
#include "AudioManager.h"

class OptionTitle : public OptionBase
{
public:
	OptionTitle(std::shared_ptr<class Settings> settings, std::shared_ptr<class AudioManager> audio);
	~OptionTitle();

	bool SystemInit() override;
	void GameInit() override;
	void Update() override;
	void Draw() const override;

	// ヘルパー
	void ResetIsOpenRules() { m_isOpenRules = false; }					// ルール説明画面を開くかどうかのフラグをリセット
	bool GetIsOpenRules() const { return m_isOpenRules; }				// ルール説明画面を開くかどうかのフラグを取得

private:
	Font m_fontRulesTitle;						// ルール説明ボタンのフォント

	bool m_isOpenRules{ false };				// ルール説明画面を開くかどうかのフラグ

	std::shared_ptr<class AudioManager> m_audio;
};
