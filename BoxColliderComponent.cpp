#include "BoxColliderComponent.h"
#include "GameObject.h"


BoxColliderComponent::BoxColliderComponent(std::weak_ptr<GameObject> owner, int updateOrder)
	:Component(owner, updateOrder)
{

}

BoxColliderComponent::~BoxColliderComponent()
{
}

Box BoxColliderComponent::GetBox() const
{
	auto ownerPtr = GetOwner().lock();
	if (!ownerPtr)
	{
		return Box{ Vec3::Zero(), m_size };
	}

	return Box{ ownerPtr->GetPosition(), m_size };
}

void BoxColliderComponent::Draw() const
{
	//DrawDebug();
}
