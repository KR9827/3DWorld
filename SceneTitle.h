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

};
