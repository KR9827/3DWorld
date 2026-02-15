#include "Explosion.h"
#include "SceneGame.h"

Explosion::Explosion(SceneGame* game)
	: GameObject(game)
{

}

void Explosion::Initialize()
{
	const int32 particleCount{ 100 };

	for (int32 i = 0; i < particleCount; ++i)
	{
		Vec3 dir = RandomVec3();					// ランダム方向
		dir.normalize();							// 正規化して単位ベクトルにする

		double speed = Random(15.0, 80.0);

		Particle p;
		p.position = GetPosition();
		p.velocity = dir * speed;
		p.life = 0.0;
		p.maxLife = Random(0.5, 1.5);

		m_particles << p;
	}
}

void Explosion::UpdateGameObject(float deltaTime)
{
	for (auto& p : m_particles)
	{
		p.life += deltaTime;
		p.position += p.velocity * deltaTime;

		// 少し減速させる
		p.velocity *= 0.95;
	}

	// 全部のパーティクルが寿命を終えたら爆発も消す
	bool allDead = true;
	for (const auto& p : m_particles)
	{
		if (p.life < p.maxLife)
		{
			allDead = false;
			break;
		}		
	}

	if (allDead)
	{
		SetState(Dead);
	}
}

void Explosion::DrawGameObject() const
{
	ScopedRenderStates3D blend{ BlendState::Additive };

	double flashTime = 0.1;

	if (!m_particles.isEmpty())
	{
		double t = m_particles[0].life; // 全体の経過時間代わり

		if (t < flashTime)
		{
			double alpha = 1.0 - (t / flashTime);

			Sphere{ GetPosition(), 10.0 + t * 80.0 }.draw(ColorF{ 1.0, 1.0, 1.0, alpha });
		}
	}

	for (const auto& p : m_particles)
	{
		double ratio = 1.0 - (p.life / p.maxLife);
		if (ratio <= 0.0) continue;

		double size = 0.5 + (1.0 - ratio) * 3.0;

		Sphere{ p.position, size }.draw(ColorF{ 1.0, 0.5, 0.0, ratio });
	}
}

bool Explosion::isFinished() const
{
	for (const auto& p : m_particles)
	{
		if (p.life < p.maxLife)
		{
			return false;
		}
	}

	return true;
}
