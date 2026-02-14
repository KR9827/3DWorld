#pragma once
#include <Siv3D.hpp>
#include "GameObject.h"

class Enemy : public GameObject
{
public:
	Enemy(class SceneGame* game);
	~Enemy();

	void Initialize() override;
	void UpdateGameObject(float deltaTime) override;

private:
	Vec3 m_size;
};
