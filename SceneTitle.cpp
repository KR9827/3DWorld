#include "SceneTitle.h"

SceneTitle::SceneTitle(const InitData& init)
	: IScene(init)
{
	SystemInit();
	GameInit();
}

SceneTitle::~SceneTitle()
{

}

bool SceneTitle::SystemInit()
{
	// フォントのパス設定
	const FilePath path = (FileSystem::GetFolderPath(SpecialFolder::SystemFonts) + U"HGRSGU.TTC");		// Windows内の
	// タイトル画面のフォント設定
	m_titleFont = { FontMethod::MSDF, 80, path };

	// 選択肢の文字設定
	m_selectNames << Font{ FontMethod::MSDF, 20, path }(U"スタート");
	m_selectNames << Font{ FontMethod::MSDF, 20, path }(U"設定");
	m_selectNames << Font{ FontMethod::MSDF, 20, path }(U"終了");

	// 終了画面の選択肢の文字設定
	m_exitNames << Font{ FontMethod::MSDF, 20 }(U"はい");
	m_exitNames << Font{ FontMethod::MSDF, 20 }(U"いいえ");

	// 終了画面内のフォント
	m_exitFont = { FontMethod::MSDF, 60 };

	return true;
}

void SceneTitle::GameInit()
{
	m_titleState = TitleState::Init;

	// 選択バーの位置の設定
	for (int i = 0; i < OPTION_NUM; ++i)
	{
		RectF rect = RectF{ Arg::center(m_selectPos + m_selectInterval * i), m_selectSize };					// 中心座標で位置を設定
		m_selectBars << rect;
	}

	// 終了画面の選択肢
	const double exitR{ 6.0 };
	for (int i = 0; i < 2; ++i)
	{
		RectF rect{ Arg::center(m_exitPos + m_exitInterval * i), m_exitSize };
		m_exitBtns << rect.rounded(exitR);
	}

	// 終了画面の背景覆いの設定
	const Vec2 coverPos{ 0.0, 0.0 };
	const Vec2 coverSize{ Scene::Width(), Scene::Height() };
	m_exitBgCover = RectF{ coverPos, coverSize };

	// 現在マウスがのってるボタンの番号
	m_currentHoverIndex = -1;
}

void SceneTitle::update()
{
	if (KeyEnter.down())
	{
		changeScene(SceneState::GAME);
	}


	switch (m_titleState)
	{
	case TitleState::Init:
		UpdateStateInit();
		break;

	case TitleState::Option:
		break;

	case TitleState::Exit:
		UpdateStateExit();
		break;
	}
}

void SceneTitle::draw() const
{
	Scene::SetBackground(ColorF{ Palette::Yellowgreen });

	// タイトル文字の描画
	const Vec2 titlePos{ Scene::Center().x, Scene::Height() / 6.0 };	// タイトル文字の位置
	const double p{ 0.4 };												// 文字の輪郭の大きさ
	m_titleFont(U"はじめての3D").drawAt(TextStyle::Outline(p, ColorF{ Palette::White }), titlePos, ColorF{ Palette::Darkorange });

	switch (m_titleState)
	{
	case TitleState::Init:
		DrawStateInit();
		break;

	case TitleState::Option:
		break;

	case TitleState::Exit:
		DrawStateExit();
		break;
	}
}

void SceneTitle::UpdateStateInit()
{
	bool isHovered = false;			// どのボタンにものってないかのフラグ

	for (size_t i = 0; i < m_selectBars.size(); ++i)
	{
		// バーのクリック判定を形に合わせる
		const RectF& optionBar = m_selectBars[i];
		//const Quad optionBar = rect.skewedX(-30_deg);
		//auto& data = getData();

		// 選択肢の上にマウスがきたらSEを流す
		if (optionBar.mouseOver())
		{
			isHovered = true;

			if (m_currentHoverIndex != i)
			{
				//data.audio->PlaySE(U"Select");
				m_currentHoverIndex = static_cast<int32>(i);
			}
		}

		if (optionBar.leftReleased())
		{
			// 決定SEを流す
			//data.audio->PlaySE(U"Decision");

			// クリックされたバーによって変える
			switch (i)
			{
			case 0:										//スタート 
				changeScene(SceneState::GAME);
				//getData().audio->StopBGM(1s);

				return;

			case 1:										// オプション
				m_titleState = TitleState::Option;

				return;

			case 2:										// 終了
				m_titleState = TitleState::Exit;

				return;
			}
		}
	}

	// ボタンの上から外れたらリセットする
	if (!isHovered)
	{
		m_currentHoverIndex = -1;
	}
}

void SceneTitle::UpdateStateExit()
{
	bool isHovered = false;			// どのボタンにものってないかのフラグ

	for (size_t i = 0; i < m_exitBtns.size(); ++i)
	{
		//auto& data = getData();
		// 選択肢の上にマウスがきたらSEを流す
		if (m_exitBtns[i].mouseOver())
		{
			isHovered = true;

			if (m_currentHoverIndex != i)
			{
				//data.audio->PlaySE(U"Select");
				m_currentHoverIndex = static_cast<int32>(i);
			}
		}

		if (m_exitBtns[i].leftReleased())
		{
			//getData().audio->PlaySE(U"Decision");

			if (i == 0)
			{
				System::Exit();
			}
			else
			{
				m_titleState = TitleState::Init;
			}
		}
	}

	// ボタンの上から外れたらリセットする
	if (!isHovered)
	{
		m_currentHoverIndex = -1;
	}
}

void SceneTitle::DrawStateInit() const
{
	// 長方形の左右の辺を-30°傾けた平行四辺形を描く
	for (const auto& optionBar : m_selectBars)
	{
		//const Quad optionBar = rect.skewedX(-30_deg);

		optionBar.draw(ColorF{ Palette::Lightgray, optionBar.mouseOver() ? 1.0 : 0.0});					// バー（平行四辺形）の描画

		// マウスが図形の上にきたら周りが白くなる
		const double thickness{ 3.0 };						// 枠線の太さ
		//optionBar.drawFrame(thickness, optionBar.mouseOver() ? Palette::White : Palette::Lightgray);
	}

	// 選択肢の名前の描画
	for (size_t i = 0; i < m_selectNames.size(); ++i)
	{
		//const Vec2 addPos{ Scene::Width() / 40.0, m_selectSize.y / 8.0 };
		m_selectNames[i].drawAt(m_selectPos + m_selectInterval * i, Palette::Sienna);
	}
}

void SceneTitle::DrawStateExit() const
{
	// 背景覆いの描画
	m_exitBgCover.draw(ColorF{ 0.0, 0.4 });

	const Vec2 fontPos{ Scene::Center().x, Scene::Height() / 3.0 };
	m_exitFont(U"ゲームを終了しますか？").drawAt(fontPos, ColorF{ Palette::White });

	for (size_t i = 0; i < m_exitBtns.size(); ++i)
	{
		const double thickness{ 2.0 };						// 枠線の太さ
		switch (i)
		{
		case 0:
			m_exitBtns[i].draw(Palette::Red);

			m_exitBtns[i].drawFrame(thickness, m_exitBtns[i].mouseOver() ? Palette::White : Palette::Red);			// マウスが図形の上にきたら周りが白くなる
			m_exitNames[i].drawAt(m_exitPos + m_exitInterval * i, ColorF{ Palette::Black });							// 文字の描画

			break;

		case 1:
			m_exitBtns[i].draw(Palette::Gray);

			m_exitBtns[i].drawFrame(thickness, m_exitBtns[i].mouseOver() ? Palette::White : Palette::Gray);			// マウスが図形の上にきたら周りが白くなる
			m_exitNames[i].drawAt(m_exitPos + m_exitInterval * i, ColorF{ Palette::Black });						// 文字の描画

			break;
		}
	}
}
