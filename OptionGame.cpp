#include "OptionGame.h"

OptionGame::OptionGame(std::shared_ptr<Settings> settings, std::shared_ptr<AudioManager> audio)
	:OptionBase(settings, audio)		// 基底クラスのコンストラクタを呼ぶ（初期化）
	,m_audio(audio)
{
	SystemInit();
	GameInit();
}

OptionGame::~OptionGame()
{

}

bool OptionGame::SystemInit()
{
	// SE
	m_audio->PreLoadSE(U"Decision", U"Assets/Sounds/SE/Decision.wav");

	// フォントのパス設定
	const FilePath path = (FileSystem::GetFolderPath(SpecialFolder::SystemFonts) + U"HGRSGU.TTC");		// Windows内のHGP創英角ポップ体
	// タイトル画面のフォント設定
	m_fontReturnTitle = { FontMethod::MSDF, 30, path };

	return true;
}

void OptionGame::GameInit()
{
	m_isReturnTitle = false;

	// オプション画面下方のボタンの位置とサイズ設定
	const Vec2 size = GetOptionWindowSize();
	const Vec2 pos = GetOptionWindowPos();
	m_btnPos = { pos.x + size.x / 2.0, Scene::Height() - pos.y * 2.5 };				// オプション画面の下方ボタンの中心位置
	m_btnSize = { size.x / 4.0, size.y / 10.0 };									// オプション画面の下方ボタンの縦幅と横幅
	m_btn = RectF{ Arg::center(m_btnPos), m_btnSize };								// ボタンの定義

	m_wasMouseOver = false;
}

void OptionGame::Update()
{
	OptionBase::Update();

	// 下方のボタンにマウスがのったらSEを流す
	if (m_btn.mouseOver())
	{
		if (!m_wasMouseOver)
		{
			m_audio->PlaySE(U"Select");
			m_wasMouseOver = true;
		}
	}
	else
	{
		m_wasMouseOver = false;
	}

	if (m_btn.leftClicked())
	{
		m_isReturnTitle = true;

		m_audio->PlaySE(U"Decision");
	}
}

void OptionGame::Draw() const
{
	OptionBase::Draw();

	m_btn.draw(m_btn.mouseOver() ? Palette::Red : Palette::Darkred);

	// タイトルに戻る文字の描画
	m_fontReturnTitle(U"タイトルに戻る").drawAt(m_btnPos, m_btn.mouseOver() ? Palette::White : Palette::Gainsboro);
}
