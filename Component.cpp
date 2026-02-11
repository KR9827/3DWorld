#include "Component.h"
#include "GameObject.h"

Component::Component(std::weak_ptr<GameObject> owner, int updateOrder)
	: m_owner(owner)
	, m_updateOrder(updateOrder)
{

}

Component::~Component()
{
	if (auto ownerPtr = m_owner.lock())
	{
		ownerPtr->RemoveComponent(std::shared_ptr<Component>(this));
	}
}

void Component::Initialize()
{

}

void Component::Update(double deltaTime)
{

}

void Component::Draw() const
{

}
