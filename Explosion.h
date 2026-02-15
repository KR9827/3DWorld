#pragma once
#include <Siv3D.hpp>
#include "GameObject.h"

struct Particle
{
	Vec3 position;
	Vec3 velocity;
	double life;
	double maxLife;
};

class Explosion : public GameObject
{
public:
	Explosion(class SceneGame* game);

	void Initialize() override;
	void UpdateGameObject(float deltaTime) override;
	void DrawGameObject() const override;

	bool isFinished() const;

private:
	Array<Particle> m_particles;


};
