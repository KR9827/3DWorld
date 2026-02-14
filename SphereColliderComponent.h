#pragma once
#include <Siv3D.hpp>
#include "Component.h"
#include "GameObject.h"

class SphereColliderComponent : public Component
{
public:
	SphereColliderComponent(std::weak_ptr<class GameObject> owner, int updateOrder = 100);
	~SphereColliderComponent();

	Sphere GetSphere() const ;

	double m_radius{ 2.0 };

	void Draw() const override;

#if _DEBUG
	// デバッグ描画（任意）
	void DrawDebug(const ColorF& color = ColorF{ 0.0, 0.0, 1.0, 0.1 }) const
	{
		GetSphere().draw(color);
	}
#endif
};
