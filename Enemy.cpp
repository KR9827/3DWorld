#include "Enemy.h"
#include "SceneGame.h"
#include "BoxColliderComponent.h"
#include "BoxRendererComponent.h"

Enemy::Enemy(SceneGame* game)
	: GameObject(game)
{

}

Enemy::~Enemy()
{
}

void Enemy::Initialize()
{
	AddComponent<BoxRendererComponent>(100);
	auto collider = AddComponent<BoxColliderComponent>(120);

	// 敵(ボックス)のサイズを設定
	m_size = Vec3{ 5.0, 10.0, 5.0 };

	// コライダーにサイズを渡す
	if (collider)
	{
		collider->SetSize(m_size);
	}

	// コンポーネントの初期化
	auto comps = GetAllComponents();
	for (const auto& comp : comps)
	{
		comp->Initialize();
	}
}

void Enemy::UpdateGameObject(float deltaTime)
{
	
}
