#pragma once
#include <Siv3D.hpp>
#include "SceneController.h"

class SceneTitle : public App::Scene
{
public:
	SceneTitle(const InitData& init);
	~SceneTitle();

	bool SystemInit();
	void GameInit();
	void update() override;
	void draw() const override;

private:
	static constexpr int32 OPTION_NUM{ 3 };

	enum class TitleState
	{
		Init,
		Option,
		Exit,

		Max,
	};
	TitleState m_titleState;

	Array<RectF> m_selectBars;							// 選択肢のバー群
	Array<DrawableText> m_selectNames;					// 選択肢の名前群
	RectF m_exitBgCover;								// 終了画面の背景覆い
	Array<RoundRect> m_exitBtns;						// 終了選択肢のボタン群
	Array<DrawableText> m_exitNames;					// 終了選択肢の名前群
	int32 m_currentHoverIndex;							// 現在マウスがのっている選択肢の番号

	// 選択肢の位置や大きさの定数
	const Vec2 m_selectPos{ Scene::Width() / 2.0, Scene::Height() * 3.0 / 5.0 };							// 選択肢の中央座標
	const Vec2 m_selectSize{ Scene::Width() / 3.0, Scene::Height() / 22.0 };								// 選択肢の縦と横の幅
	const Vec2 m_selectInterval{ 0.0, Scene::Height() / 12.0 };												// 選択肢の縦の間隔
	const Vec2 m_exitPos{ Scene::Center().x - Scene::Width() / 6.0, Scene::Center().y + Scene::Height() / 10.0 };
	const Vec2 m_exitSize{ Scene::Width() / 10.0, Scene::Height() / 10.0 };
	const Vec2 m_exitInterval{ Scene::Width() / 3.0 , 0 };

	Font m_titleFont;									// タイトルのフォント
	Font m_exitFont;									// 終了選択肢のフォント


	void UpdateStateInit();
	void UpdateStateExit();
	void DrawStateInit() const;
	void DrawStateExit() const;




};
