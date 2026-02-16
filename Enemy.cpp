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
	//AddComponent<BoxRendererComponent>(100);
	auto collider = AddComponent<BoxColliderComponent>(120);

	m_rockModel = Model{ U"Assets/obj/untitled.obj" };
	Model::RegisterDiffuseTextures(m_rockModel, TextureDesc::MippedSRGB);

	// HPの初期化
	m_maxHP = 300;
	m_HP = m_maxHP;

	// HPバーの設定	
	m_HPBarMaxSize = Vec2{ Scene::Width() * 7.0 / 10.0, 15.0 };
	m_HPBarSize = m_HPBarMaxSize;
	m_HPBarPos = Vec2{ (Scene::Width() - m_HPBarMaxSize.x) / 2.0, Scene::Height() * 9.0 / 10.0};
	m_HPBarBack = RectF{ m_HPBarPos, m_HPBarMaxSize }.rounded(m_HPBarRadius);
	m_HPBarFront = RectF{ m_HPBarPos, m_HPBarMaxSize }.rounded(m_HPBarRadius);

	// 敵(ボックス)のサイズを設定
	m_size = Vec3{ 10.0, 8.0, 10.0 };

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
	m_ratio = static_cast<double>(m_HP) / m_maxHP;
	double hpBarFrontWidth = m_HPBarMaxSize.x * m_ratio;
	m_HPBarSize.x = hpBarFrontWidth;
	m_HPBarFront = RectF{ m_HPBarPos, m_HPBarSize }.rounded(m_HPBarRadius);
}

void Enemy::DrawGameObject() const
{
	const Vec3 offset{ 11.0, 2.0, 0.0 };
	// 岩のサイズと大きさを変更する計算
	Mat4x4 world = Mat4x4::Scale(this->GetScale()) * Mat4x4::Translate(offset) * Mat4x4::Translate(this->GetPosition());

	m_rockModel.draw(world);
}

void Enemy::TakeDamage(const int32& damage)
{
	m_HP -= damage;
	if (m_HP <= 0)
	{
		m_HP = 0;
	}
}

void Enemy::HPBarDraw() const
{
	// HPバーの背景
	m_HPBarBack.draw(ColorF{ Palette::Black, 0.7 });

	// HPバーの前景
	m_HPBarFront.draw(ColorF{ m_ratio < 0.1 ? Palette::Red : m_ratio < 0.3 ? Palette::Yellow : Palette::Green });
}
