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
	void DrawGameObject() const override;

	void TakeDamage(const int32& damage);


	void HPBarDraw() const;

	int32 GetHP() const { return m_HP; }

private:
	Model m_rockModel;

	Vec3 m_size;

	int32 m_HP;
	int32 m_maxHP;

	// HPバー変数用
	RoundRect m_HPBarBack;
	RoundRect m_HPBarFront;
	Vec2 m_HPBarPos;
	Vec2 m_HPBarSize;
	Vec2 m_HPBarMaxSize;
	const double m_HPBarRadius{ 10.0 };
	double m_ratio{ 0.0 };
};
