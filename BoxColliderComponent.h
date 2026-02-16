#pragma once
#include <Siv3D.hpp>
#include "Component.h"

class BoxColliderComponent : public Component
{
public:
	BoxColliderComponent(std::weak_ptr<class GameObject> owner, int updateOrder = 100);
	~BoxColliderComponent();

	Box GetBox() const;
	void Draw() const override;

	void SetSize(const Vec3& size) { m_size = size; }

	Vec3 m_size{ 1.0, 1.0, 1.0 };

	

#if _DEBUG
	// デバッグ描画（任意）
	void DrawDebug(const ColorF& color = ColorF{ 0.0, 1.0, 0.0, 0.2 }) const
	{
		GetBox().draw(color);
	}
#endif
};
