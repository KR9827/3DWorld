#pragma once
#include <Siv3D.hpp>
#include "SceneController.h"

class SceneResult : public App::Scene
{
public:
	SceneResult(const InitData& init);
	~SceneResult();
	
	bool SystemInit();
	void GameInit();
	void update() override;
	void draw() const override;

private:
	Array<RectF> m_selectBars;							// 選択肢のバー群
	Array<DrawableText> m_selectNames;					// 選択肢の名前群
	RectF m_bgCover;									// 終了画面の背景覆い

	const Vec2 m_selectPos{ Scene::Width() * 3.0 / 10.0, Scene::Height() * 3.0 / 5.0 };
	const Vec2 m_selectInterval{ (Scene::Width() / 2.0 - m_selectPos.x) * 2.0, 0.0 };
	const Vec2 m_selectSize{ Scene::Width() / 3.0, Scene::Height() / 18.0 };

	Font m_resultFont;							// 結果のフォント

	Texture m_clearBG;
	Texture m_FailedBG;

	double m_timer{ 0.0 };						// 結果表示から選択肢が表示されるまでの時間
	bool m_isShowSelect{ false };				// 選択肢を表示するか
};
