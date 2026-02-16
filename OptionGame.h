#pragma once
#include "OptionBase.h"
#include "Settings.h"
#include "AudioManager.h"

class OptionGame : public OptionBase
{
public:
	OptionGame(std::shared_ptr<class Settings> settings, std::shared_ptr<class AudioManager> audio);
	~OptionGame();

	bool SystemInit() override;
	void GameInit() override;
	void Update() override;
	void Draw() const override;

	// ヘルパー
	void ResetIsReturnTitle() { m_isReturnTitle = false; }			// タイトルに戻るかどうかのフラグをリセット
	bool IsReturnTitle() const { return m_isReturnTitle; }			// タイトルに戻るかどうかのフラグを取得

private:
	Font m_fontReturnTitle;						// タイトルに戻るボタンのフォント
	bool m_isReturnTitle{ false };				// タイトルに戻るかどうかのフラグ
	RectF m_btn;								// タイトルに戻るボタン
	Vec2 m_btnPos;								// オプション画面の下方ボタンの中心位置
	Vec2 m_btnSize;								// オプション画面の下方ボタンの縦幅と横幅

	std::shared_ptr<class AudioManager> m_audio;
};
