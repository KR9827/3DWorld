#include "GameObject.h"
#include "SceneGame.h"


GameObject::GameObject(SceneGame* game)
	: m_game(game)
	, m_state(Active)
	, m_position(0, 0, 0)
	, m_rotation(Quaternion::Identity())
	, m_scale(1.0f)
	, m_recomputeWorldTransform(true)
{
	
}

GameObject::~GameObject()
{
	m_components.clear();
}

void GameObject::Initialize()
{
}

void GameObject::Update(float deltaTime)
{
	if (m_state == Active)
	{
		ComputeWorldTransform();

		UpdateComponents(deltaTime);
		UpdateGameObject(deltaTime);

		ComputeWorldTransform();
	}
}

void GameObject::UpdateComponents(float deltaTime)
{
	for (auto comp : m_components)
	{
		comp->Update(deltaTime);
	}
}

void GameObject::UpdateGameObject(float)
{
}

void GameObject::Draw() const
{
	if (m_state == Active)
	{
		for (const auto& comp : m_components)
		{
			comp->Draw();
		}
	}
	else
	{
		Console << U"m_state != Active";
		return;
	}
}


void GameObject::ComputeWorldTransform()
{
	if (!m_recomputeWorldTransform) return;

	m_recomputeWorldTransform = false;

	Mat4x4 world = Mat4x4::Identity();

	// 変更されたものをscale->rotate->positionの順に変更する
	world *= Mat4x4::Scale(m_scale);
	world *= Mat4x4::Rotate(m_rotation);
	world *= Mat4x4::Translate(m_position);

	m_worldTransform = world;			// 描画、カメラなどに使われる行列

	for (auto& comp : m_components)
	{
		comp->OnUpdateWorldTransform();
	}
}


void GameObject::RemoveComponent(std::shared_ptr<Component> component)
{
	auto iter = std::find(m_components.begin(), m_components.end(), component);
	if (iter != m_components.end())
	{
		m_components.erase(iter);
	}
}
