#pragma once
#include <Siv3D.hpp>
#include "GameObject.h"

class Player : public GameObject
{
public:
	Player(class SceneGame* game);
	~Player();

	/// @brief 更新処理
	/// @param deltaTime 前フレームからの経過時間
	void UpdateGameObject(float deltaTime) override;


private:


};
