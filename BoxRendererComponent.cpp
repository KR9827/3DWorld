#include "BoxRendererComponent.h"
#include "GameObject.h"
#include "BoxColliderComponent.h"

BoxRendererComponent::BoxRendererComponent(std::weak_ptr<GameObject> owner, int updateOrder)
	: Component(owner, updateOrder)
{

}

BoxRendererComponent::~BoxRendererComponent()
{
}

void BoxRendererComponent::Initialize()
{
}

void BoxRendererComponent::Draw() const
{
	auto ownerPtr = GetOwner().lock();
	if (!ownerPtr) return;

	auto collider = ownerPtr->GetComponent<BoxColliderComponent>();
	if (collider)
	{
		collider->GetBox().draw(ColorF{ Palette::Red });
	}
}
