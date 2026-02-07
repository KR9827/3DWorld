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
	Scene::SetBackground(ColorF{ Palette::Green });

	return true;
}

void SceneResult::GameInit()
{

}

void SceneResult::update()
{
	if (KeyEnter.down())
	{
		changeScene(SceneState::TITLE);
	}
}

void SceneResult::draw() const
{

}
