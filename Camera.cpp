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


		//// 対象の回転行列
		//Mat4x4 rotationMatrix{ Mat4x4::Rotate(target->GetRotation()) };
		//
		//// オフセットを対象の向きに合わせて回転させる
		//Vec3 rotateOffset{ rotationMatrix.transformPoint(m_offset) };
		//
		//Vec3 idealPos{ target->GetPosition() + rotateOffset };
		//Vec3 currentPos{ this->GetPosition() };
		//
		//// カメラの位置(少し遅れて付いてくる)
		//this->SetPosition(Math::Lerp(currentPos, idealPos, 0.5));		// 基底クラスの関数なので、thisがあると分かりやすくなる
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
