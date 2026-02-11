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

	/// @brief AnimationComponentのポインタを取得する
	/// @param anim AnimationComponentのポインタ
	void SetAnimation(std::shared_ptr<class AnimationComponent> anim) { m_anim = anim; }

	/// @brief 移動方向を渡す
	/// @return 移動方向
	Vec3 GetMove() const { return m_move; }

private:
	static constexpr double MOVE_SPEED{ 20.0 };

	class SceneGame* m_game;
	std::shared_ptr<class AnimationComponent> m_anim;

	Vec3 m_move{ 0.0, 0.0, 0.0 };
	double m_angle{ 0.0 };

};
