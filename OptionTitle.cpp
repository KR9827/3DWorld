#include "OptionTitle.h"


OptionTitle::OptionTitle(std::shared_ptr<Settings> settings, std::shared_ptr<AudioManager> audio)
	:OptionBase(settings, audio)		// 基底クラスのコンストラクタを呼ぶ（初期化）
	,m_audio(audio)
{
	SystemInit();
	GameInit();
}

OptionTitle::~OptionTitle()
{

}

bool OptionTitle::SystemInit()
{
	// SE
	//m_audio->PreLoadSE(U"Decision", U"Assets/Sounds/SE/Decision.wav");

	// フォントのパス設定
	const FilePath path = (FileSystem::GetFolderPath(SpecialFolder::SystemFonts) + U"HGRSGU.TTC");		// Windows内のHGP創英角ポップ体
	// タイトル画面のフォント設定
	m_fontRulesTitle = { FontMethod::MSDF, 30, path };

	return true;
}

void OptionTitle::GameInit()
{
	m_isOpenRules = false;
}

void OptionTitle::Update()
{
	OptionBase::Update();

	//if (OptionBase::m_btn.leftClicked())
	//{
	//	// ルール説明画面を開く
	//	m_isOpenRules = true;
	//
	//	//m_audio->PlaySE(U"Decision");
	//}
}

void OptionTitle::Draw() const
{
	OptionBase::Draw();

	//// ルール説明ボタンの色変更
	//OptionBase::m_btn.draw(OptionBase::m_btn.mouseOver() ? Palette::Lightpink : Palette::Pink);
	//
	//// ルール説明文字の描画
	//m_fontRulesTitle(U"ルール説明").drawAt(OptionBase::m_btnPos, OptionBase::m_btn.mouseOver() ? Palette::Sienna : Palette::Chocolate);
}
