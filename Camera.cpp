#include "Camera.h"
#include "SceneGame.h"

Camera::Camera(SceneGame* game)
	: GameObject(game)
{

}

Camera::~Camera()
{
}

void Camera::UpdateGameObject(float deltaTime)
{
	// 対象の動きに合わせて回転して、常に対象の背後をキープ
	if (auto target = m_target.lock())
	{
		// 回転スピード
		const float angleSpeed = 1.5f;
		//m_angle += angleSpeed * deltaTime;
		if (KeyRight.pressed())
		{
			m_angle += angleSpeed * deltaTime;
		}
		if (KeyLeft.pressed())
		{
			m_angle -= angleSpeed * deltaTime;
		}

		Vec3 targetPos = target->GetPosition();

		Vec3 idealPos{ 0, 0, 0 };
		idealPos.x = targetPos.x + cosf(m_angle) * m_radius;
		idealPos.z = targetPos.z + sinf(m_angle) * m_radius;
		idealPos.y = targetPos.y + 8.0f;

		Vec3 currentPos = this->GetPosition();

		SetPosition(Math::Lerp(currentPos, idealPos, 0.1f));
	}
}

BasicCamera3D Camera::GetCamera() const
{
	// 自分の位置から対象の方向を見る
	Vec3 focus{ Vec3{0, 0, 0} };
	if (auto target = m_target.lock())
	{
		focus = target->GetPosition() + Vec3{ 0.0, 5.0, 0.0 };		// 注視点（対象の位置）を取得
	}

	return BasicCamera3D{ Scene::Size(), 60_deg, GetPosition(), focus};
}

Vec3 Camera::GetForward() const
{
	Vec3 f = (GetCamera().getFocusPosition() - GetPosition());
	f.y = 0;
	return f.normalized();
}

Vec3 Camera::GetRight() const
{
	Vec3 f = GetForward();
	return Vec3{ f.z, 0, -f.x };
}
