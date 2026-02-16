#include <Siv3D.hpp>
#include "OptionBase.h"


OptionBase::OptionBase(std::shared_ptr<Settings> settings, std::shared_ptr<AudioManager> audio)
	: m_settings(settings)
	, m_audio(audio)
	, m_isClosed(false)
{
	SystemInit();
	GameInit();
}


bool OptionBase::SystemInit()
{
	// SE
	m_audio->PreLoadSE(U"Select", U"Assets/Sounds/SE/select.wav");

	return true;
}

void OptionBase::GameInit()
{
	// オプション画面が閉じているかのフラグ
	m_isClosed = false;

	// オプション画面のウィンドウの位置とサイズ設定
	m_optionWindowSize = { Scene::Width() * 8.0 / 10.0 , Scene::Height() * 8.0 / 10.0 };										// 縦幅と横幅
	m_optionWindowPos = { (Scene::Width() - m_optionWindowSize.x) / 2.0, (Scene::Height() - m_optionWindowSize.y) / 2.0 };		// 左上の座標
	RectF window = RectF{ m_optionWindowPos, m_optionWindowSize };																// ウィンドウ（四角形）の定義
	const double round{ 6.0 };																									// 角の丸み
	m_optionWindow = window.rounded(round);																						// 角を丸くする

	// オプション画面の閉じるボタンの位置とサイズ設定
	m_optionCloseBtnSize = { m_optionWindowSize.x / 12.0 , m_optionWindowSize.y / 12.0 };										// 縦幅と横幅
	m_optionCloseBtnPos = { m_optionWindowPos.x + m_optionWindowSize.x - m_optionCloseBtnSize.x, m_optionWindowPos.y };			// 左上の位置
	RectF rect = RectF{ m_optionCloseBtnPos, m_optionCloseBtnSize };															// ボタンの定義
	m_optionCloseBtn = rect.rounded(0.0, round, 0.0, 0.0);																		// 右上の角を丸くする

	// 閉じるマークのフォント設定
	m_closeMark = Font{ FontMethod::MSDF, 30 };

	//// オプション画面下方のボタンの位置とサイズ設定
	//m_btnPos = { m_optionWindowPos.x + m_optionWindowSize.x / 2.0, Scene::Height() - m_optionWindowPos.y * 2.5 };				// オプション画面の下方ボタンの中心位置
	//m_btnSize = { m_optionWindowSize.x / 4.0, m_optionWindowSize.y / 5.0 };														// オプション画面の下方ボタンの縦幅と横幅
	//RectF btn = RectF{ Arg::center(m_btnPos), m_btnSize };																		// ボタンの定義
	//m_btn = btn.rounded(round);																									// 角を丸くする

	// SE用
	m_isPlayedSE = false;
}

void OptionBase::Update()
{
	// 描画された長方形を左クリックしたらゲーム画面に戻る
	if (m_optionCloseBtn.leftClicked())
	{
		m_isClosed = true;
		m_audio->PlaySE(U"Select");
	}

	//// 下方のボタンにマウスがのったらSEを流す
	//if (m_btn.mouseOver())
	//{
	//	if (!m_isPlayedSE)
	//	{
	//		//m_audio->PlaySE(U"Select");
	//		m_isPlayedSE = true;
	//	}
	//}
	else
	{
		m_isPlayedSE = false;
	}
}

void OptionBase::Draw() const
{
	// オプション画面のウィンドウの描画
	m_optionWindow.draw(ColorF{ 0.2, 0.2, 0.2 , 0.8 });

	// オプション画面の閉じるボタンの中心座標
	const double btnX = m_optionCloseBtnPos.x + m_optionCloseBtnSize.x / 2.0;
	const double btnY = m_optionCloseBtnPos.y + m_optionCloseBtnSize.y / 2.0;
	// オプション画面の閉じるボタンと×マークの描画
	m_optionCloseBtn.draw(ColorF{ 0.4, 0.4, 0.4 });
	m_closeMark(U"×").drawAt(btnX, btnY, ColorF{ Palette::Black });
	if (m_optionCloseBtn.mouseOver())
	{
		m_optionCloseBtn.draw(ColorF{ 0.6, 0.2, 0.2 });
		m_closeMark(U"×").drawAt(btnX, btnY, ColorF{ Palette::White });
	}

	// 位置の変数
	const double sliderWidth{ m_optionWindowSize.x / 3.0 };																			// スライダーの横幅
	const Vec2 sliderPos{ (m_optionWindowSize.x - sliderWidth) / 2.0, m_optionWindowPos.y + m_optionWindowSize.y / 7.0 };			// スライダーの左上の座標
	const Vec2 sliderGap{ 0, m_optionWindowSize.y / 10.0 };																			// スライダー間のサイズ
	const double sliderLabelW{ m_optionWindowSize.x / 5.0 };																		// スライダーの文字の部分のサイズ


	// 音量のスライダー
	if (SimpleGUI::Slider(U"Master Volume", m_settings->m_masterVolume, sliderPos, sliderLabelW, sliderWidth))
	{
		m_audio->SetMasterVolume(m_settings->m_masterVolume);
	}
	if (SimpleGUI::Slider(U"BGM Volume", m_settings->m_bgmVolume, sliderPos + sliderGap, sliderLabelW, sliderWidth))
	{
		m_audio->SetBusVolume(Bus::BGM, m_settings->m_bgmVolume);
	}
	if (SimpleGUI::Slider(U"SE Volume", m_settings->m_seVolume, sliderPos + sliderGap * 2.0, sliderLabelW, sliderWidth))
	{
		m_audio->SetBusVolume(Bus::SE, m_settings->m_seVolume);
	}
	// カメラ感度
	if (SimpleGUI::Slider(U"Camera Sensitivity", m_settings->m_cameraSensitivity, 0.001, 0.01, sliderPos + sliderGap * 3.0, sliderLabelW, sliderWidth))
	{
		m_settings->Save();   // 変更されたら保存
	}


	//m_btn.draw(m_btn.mouseOver() ? Palette::Red : Palette::Darkred);
}

