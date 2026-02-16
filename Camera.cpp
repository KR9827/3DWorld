#include "Camera.h"
#include "SceneGame.h"
#include "Settings.h"

Camera::Camera(SceneGame* game, std::shared_ptr<Settings> settings)
	: GameObject(game)
	, m_settings(settings)
{
	Cursor::ClipToWindow(true);
}

Camera::~Camera()
{
	Cursor::ClipToWindow(false);
}

void Camera::UpdateGameObject(float deltaTime)
{
	Cursor::RequestStyle(CursorStyle::Hidden);

	// 対象の動きに合わせて回転して、常に対象の背後をキープ
	if (auto target = m_target.lock())
	{
		bool warped = WrapCursorAtWindowEdges();

		// マウスの移動量
		const Vec2 mouseDelta = Cursor::DeltaF();

		// マウスが端に到達してないとき
		if (!warped)
		{
			const double sensitivity = m_settings->m_cameraSensitivity;

			// 感度をかける
			m_yaw -= mouseDelta.x * sensitivity;
			m_pitch += mouseDelta.y * sensitivity;
		}

		// 上下の角度を制限
		m_pitch = Clamp(m_pitch, -0.2, 1.2);

		Vec3 targetPos = target->GetPosition() + Vec3{ 0.0, 5.0, 0.0 };

		// 球面でカメラ位置を計算
		Vec3 offset{ 0, 0, 0 };
		offset.x = Math::Cos(m_pitch) * Math::Cos(m_yaw) * m_radius;
		offset.z = Math::Cos(m_pitch) * Math::Sin(m_yaw) * m_radius;
		offset.y = Math::Sin(m_pitch) * m_radius;

		Vec3 idealPos = targetPos + offset;

		Vec3 currentPos = this->GetPosition();

		float lerpFactor = 1.0f - pow(0.001f, deltaTime);
		Vec3 smoothPos = Math::Lerp(currentPos, idealPos, lerpFactor);
		smoothPos += m_shakeOffset;
		SetPosition(smoothPos);
	}

	// カメラの揺れの処理
	if (m_shakeTime < m_shakeDuration)
	{
		m_shakeTime += deltaTime;

		double progress = 1.0 - (m_shakeTime / m_shakeDuration);

		m_shakeOffset.x = 0;
		m_shakeOffset.y = Random(-1.0, 1.0) * m_shakePower * progress;
		m_shakeOffset.z = 0;
	}
	else
	{
		m_shakeOffset = Vec3::Zero();
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

bool Camera::WrapCursorAtWindowEdges()
{
	const Size size = Scene::Size();
	Point pos = Cursor::Pos();
	const double margin = 4.0;

	bool warped = false;

	// 左右
	if (pos.x <= margin)
	{
		pos.x = size.x - margin - 2.0;
		warped = true;
	}
	else if (pos.x >= size.x - margin)
	{
		pos.x = margin + 2.0;
		warped = true;
	}

	// 上下
	if (pos.y <= margin)
	{
		pos.y = size.y - margin - 2.0;
		warped = true;
	}
	else if (pos.y >= size.y - margin)
	{
		pos.y = margin + 2.0;
		warped = true;
	}

	if (warped)
	{
		Cursor::SetPos(pos);
	}

	return warped;
}

void Camera::StartShake(double duration, double power)
{
	m_shakeDuration = duration;
	m_shakeTime = 0.0;
	m_shakePower = power;
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

