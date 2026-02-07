#include "Player.h"
#include "SceneGame.h"

Player::Player(SceneGame* game)
	: GameObject(game)
{

}

Player::~Player()
{

}

void Player::UpdateGameObject(float deltaTime)
{
	// 仮のプレイヤーの移動
	if (KeyW.pressed())
	{
		// 現在の向きを取得
		Quaternion q{ GetRotation() };
		// 正面（ｚ軸）を現在の回転で回して、進むべき方向を出す
		Vec3 forward{ q * Vec3::Forward() };
		// 現在の位置に足して更新
		SetPosition(GetPosition() + (forward * deltaTime));
	}


	// 仮のプレイヤーの左右回転
	//if (KeyRight.pressed())
	//{
	//	double rotateSpeed{ 90_deg * Scene::DeltaTime() };
	//	SetRotation(GetRotation() * Quaternion::RotateY(rotateSpeed));
	//}
	//if (KeyLeft.pressed())
	//{
	//	double rotateSpeed{ 90_deg * Scene::DeltaTime() };
	//	SetRotation(GetRotation() * Quaternion::RotateY(-rotateSpeed));
	//}
}
