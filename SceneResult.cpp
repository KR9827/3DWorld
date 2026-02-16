#include "SceneResult.h"

SceneResult::SceneResult(const InitData& init)
	: IScene(init)
{
	SystemInit();
	GameInit();
}

SceneResult::~SceneResult()
{

}

bool SceneResult::SystemInit()
{
	// フォントのパス設定
	const FilePath path = (FileSystem::GetFolderPath(SpecialFolder::SystemFonts) + U"HGRSGU.TTC");		// Windows内のfont
	m_resultFont = Font{ FontMethod::MSDF, 100, path };

	// 終了画面の選択肢の文字設定
	m_selectNames << Font{ FontMethod::MSDF, 40 }(U"リトライ");
	m_selectNames << Font{ FontMethod::MSDF, 40 }(U"タイトルに戻る");

	// 背景画像のロード
	Image image = Image{ U"Assets/texture/ClearBG.png" };
	m_clearBG = Texture{ image };
	image = Image{ U"Assets/texture/FailedBG.png" };
	m_FailedBG = Texture{ image };
	image.release();

	return true;
}

void SceneResult::GameInit()
{
	// 選択バーの位置の設定
	for (size_t i = 0; i < m_selectNames.size(); ++i)
	{
		RectF rect = RectF{ Arg::center(m_selectPos + m_selectInterval * i), m_selectSize };					// 中心座標で位置を設定
		m_selectBars << rect;
	}

	// 終了画面の背景覆いの設定
	const Vec2 coverPos{ 0.0, 0.0 };
	const Vec2 coverSize{ Scene::Size() };
	m_bgCover = RectF{ coverPos, coverSize };

	m_timer = 0.0;
	m_isShowSelect = false;
}

void SceneResult::update()
{
	const double dt = Scene::DeltaTime();

	m_timer += dt;
	if (m_timer >= 2.0 || MouseL.up())
	{
		m_isShowSelect = true;
	}

	for (size_t i = 0; i < m_selectBars.size(); ++i)
	{
		if (m_selectBars[i].mouseOver())
		{
			// se流す


			if (MouseL.down())
			{
				if (i == 0)
				{
					changeScene(SceneState::GAME);
				}
				else
				{
					changeScene(SceneState::TITLE);
				}

				m_timer = 0.0;
				m_isShowSelect = false;
			}
		}		
	}
}

void SceneResult::draw() const
{
	Scene::SetBackground(ColorF{ Palette::Green });

	

	// 結果の表示
	const auto& data = getData();

	const Vec2 pos{ 0.0, 0.0 };
	data.isClear ? m_clearBG.draw(pos) : m_FailedBG.draw(pos);

	const String resultText = data.isClear ? U"クリア！！" : U"失敗...";
	const ColorF resultColor = data.isClear ? ColorF{ Palette::Gold } : ColorF{ Palette::Firebrick };
	const Vec2 fontPos{ Scene::Width() / 2.0, Scene::Height() * 3.0 / 10.0 };
	m_resultFont(resultText).drawAt(fontPos, resultColor);

	if (m_isShowSelect)
	{
		// 背景覆いの描画
		m_bgCover.draw(ColorF{ 0.0, 0.4 });

		// 選択肢の描画
		for (const auto& bar : m_selectBars)
		{
			bar.draw(ColorF{ Palette::Lightgray, bar.mouseOver() ? 1.0 : 0.0 });
		}
		// 選択肢の名前の描画
		for (size_t i = 0; i < m_selectNames.size(); ++i)
		{
			m_selectNames[i].drawAt(m_selectPos + m_selectInterval * i, ColorF{ Palette::Darkblue });
		}
	}
}
