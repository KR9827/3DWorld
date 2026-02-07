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

};
