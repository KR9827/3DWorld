#pragma once

class Component
{
public:
	/// @brief コンストラクタ
	/// @param owner コンポーネントを付ける対象
	/// @param updateOrder このコンポーネントを更新する時の優先度
	Component(std::weak_ptr<class GameObject> owner, int updateOrder = 100);
	virtual ~Component();

	virtual void Initialize();
	virtual void Update(double deltaTime);
	virtual void Draw() const;
	virtual void OnUpdateWorldTransform() {}

	std::weak_ptr<class GameObject> GetOwner() { return m_owner; }
	int GetUpdateOrder() const { return m_updateOrder; }
protected:
	std::weak_ptr<class GameObject> m_owner;

	int m_updateOrder;			// コンポーネントを更新する優先度
};
