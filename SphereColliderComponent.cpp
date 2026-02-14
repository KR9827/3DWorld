#include "SphereColliderComponent.h"

SphereColliderComponent::SphereColliderComponent(std::weak_ptr<GameObject> owner, int updateOrder)
	:Component(owner, updateOrder)
{

}

SphereColliderComponent::~SphereColliderComponent()
{
}

Sphere SphereColliderComponent::GetSphere() const
{
	auto ownerPtr = GetOwner().lock();
	if (!ownerPtr)
	{
		return Sphere{ Vec3::Zero(), m_radius };
	}

	const Vec3 offset{ 0.0, 2.0, 0.0 };
	return Sphere{ ownerPtr->GetPosition() + offset, m_radius };
}

void SphereColliderComponent::Draw() const
{
	DrawDebug();
}
