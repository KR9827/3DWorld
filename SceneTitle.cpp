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
	Scene::SetBackground(ColorF{ Palette::Yellowgreen });

	return true;
}

void SceneTitle::GameInit()
{

}

void SceneTitle::update()
{
	if (KeyEnter.down())
	{
		changeScene(SceneState::GAME);
	}
}

void SceneTitle::draw() const
{

}
