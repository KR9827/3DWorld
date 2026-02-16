#pragma once
#include "Settings.h"
#include "AudioManager.h"
#include "Application.h"

class OptionBase
{
public:
	OptionBase(std::shared_ptr<class Settings> settings, std::shared_ptr<class AudioManager> audio);
	virtual ~OptionBase() = default;

	virtual bool SystemInit();
	virtual void GameInit();
	virtual void Update();
	virtual void Draw() const;

	void ResetIsClosed() { m_isClosed = false; }			// set関数
	bool IsClosed() const { return m_isClosed; }			// オプション画面を閉じる判定用

	Vec2 GetOptionWindowPos() const { return m_optionWindowPos; }
	Vec2 GetOptionWindowSize() const { return m_optionWindowSize; }

private:
	bool m_isClosed;							// オプション画面を閉じるかどうかのフラグ

	Vec2 m_optionWindowPos;						// オプション画面のウィンドウの左上の座標
	Vec2 m_optionWindowSize;					// オプション画面のウィンドウの縦と横のサイズ
	RoundRect m_optionWindow;					// オプション画面のウィンドウ
	Vec2 m_optionCloseBtnPos;					// オプション画面の閉じるボタンの左上の座標
	Vec2 m_optionCloseBtnSize;					// オプション画面の閉じるボタンの縦と横のサイズ
	Polygon m_optionCloseBtn;					// オプション画面の閉じるボタン
	Font m_closeMark;							// オプション画面の×マーク

	// SE用
	bool m_isPlayedSE{ false };					// SEがなったかのフラグ

	// 各クラスのインスタンスのポインタ
	std::shared_ptr<class Settings> m_settings;
	std::shared_ptr<class AudioManager> m_audio;
};
