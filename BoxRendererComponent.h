#pragma once
#include <Siv3D.hpp>
#include "Component.h"

class BoxRendererComponent : public Component
{
public:
	BoxRendererComponent(std::weak_ptr<class GameObject> owner, int updateOrder = 100);
	~BoxRendererComponent();

	void Initialize() override;
	void Draw() const override;


private:
	Box m_box;
};
