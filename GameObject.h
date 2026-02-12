#pragma once
#include <Siv3D.hpp>
#include "Component.h"

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
	enum State
	{
		Active,
		Paused,
		Dead,
	};

	GameObject(class SceneGame* game);
	virtual ~GameObject();

	void Update(float deltaTime);
	void UpdateComponents(float deltaTime);
	virtual void UpdateGameObject(float deltaTime);

	void Draw() const;

	// ===== ゲッター/セッター
	// 位置
	const Vec3& GetPosition() const { return m_position; }
	void SetPosition(const Vec3& pos) { m_position = pos; m_recomputeWorldTransform = true; }
	// スケール
	float GetScale() const { return m_scale; }
	void SetScale(float scale) { m_scale = scale; m_recomputeWorldTransform = true; }
	// 回転
	const Quaternion& GetRotation() const { return m_rotation; }
	void SetRotation(const Quaternion& rotation) { m_rotation = rotation; m_recomputeWorldTransform = true; }

	void ComputeWorldTransform();
	const Mat4x4 GetWorldTransform() const { return m_worldTransform; }

	class SceneGame* GetSceneGame() { return m_game; }

	// コンポーネントを追加するテンプレート関数
	template <class T, class... Args>
	std::shared_ptr<T> AddComponent(Args&&... args)
	{
		// コンポーネントを生成する
		auto comp = std::make_shared<T>(shared_from_this(), std::forward<Args>(args)...);			// ここで自動的に自身のポインタを渡している

		// 適切な位置に挿入
		int myOrder = comp->GetUpdateOrder();
		auto iter = m_components.begin();
		for (; iter != m_components.end(); ++iter)		// 初期化を外に出す書き方、こうすることでiterをforの外でも使える
		{
			if (myOrder < (*iter)->GetUpdateOrder())
			{
				break;
			}
		}
		m_components.insert(iter, comp);

		return comp;
	}

	// ついてるコンポーネントを取得するテンプレート関数
	template <class T, class... Args>
	std::shared_ptr<T> GetComponent(Args&&... args)
	{
		for (const auto& comp : m_components)
		{
			auto casted = std::dynamic_pointer_cast<T>(comp);
			if (casted)
			{
				return casted;
			}
		}
		return nullptr;
	}

	// コンポーネントを削除する
	void RemoveComponent(std::shared_ptr<class Component> component);

private:
	State m_state;

	// Transform
	Mat4x4 m_worldTransform;
	Vec3 m_position;
	Quaternion m_rotation;
	float m_scale;
	bool m_recomputeWorldTransform;				// Transformが変わったかどうか

	Array<std::shared_ptr<class Component>> m_components;
	class SceneGame* m_game;								// GameObjectの寿命がSceneGameと逆転することはないから速度の速い生ポインタ

};
