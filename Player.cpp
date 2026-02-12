#include "Player.h"
#include "SceneGame.h"
#include "Camera.h"
#include "AnimationComponent.h"

Player::Player(SceneGame* game)
	: GameObject(game)
	, m_game(game)
{

}

Player::~Player()
{

}

void Player::UpdateGameObject(float deltaTime)
{
	auto camera = m_game->GetCamera();
	if (!camera) return;

	auto anim = GetComponent<AnimationComponent>();
	if (!anim) return;

	// ---------------------------
	// 攻撃
	// ---------------------------
	if (MouseL.down())
	{
		if (!anim->IsPlayingAttack())
		{
			// 攻撃開始
			m_comboIndex = 1;
			anim->Play(U"Attack1", false);
		}
		else
		{
			// 次の攻撃を予約
			m_comboQueued = true;
		}
	}

	// 攻撃中
	if (anim->IsPlayingAttack())
	{
		if (m_comboQueued && anim->GetRemainingTime() < 0.2f)
		{
			if (m_comboIndex < 10)
			{
				m_comboIndex++;
				String next = U"Attack" + Format(m_comboIndex);
				anim->Play(next, false);
				m_comboQueued = false;
			}
		}

		if (anim->IsFinishedAttack())
		{
			// コンボ終了
			m_comboIndex = 0;
			m_comboQueued = false;
		}

		return;
	}


	// --------------------------
	// 移動処理
	// --------------------------
	// 何も押されてない時は止まる
	m_move = { 0, 0, 0 };

	// プレイヤーの移動方向(カメラから見た方向)
	if (KeyW.pressed()) m_move += camera->GetForward();
	if (KeyS.pressed()) m_move -= camera->GetForward();
	if (KeyD.pressed()) m_move += camera->GetRight();
	if (KeyA.pressed()) m_move -= camera->GetRight();

	// 移動してる時
	if (m_move.lengthSq() > 0)
	{
		// 移動速度の設定
		m_move = m_move.normalized();
		this->SetPosition(this->GetPosition() + m_move * MOVE_SPEED * deltaTime);

		// プレイヤーの向きを移動方向に合わせる
		double targetAngle{ std::atan2(m_move.x, m_move.z) };			// arctanで角度を取得

		// 角度補間
		double diff{ targetAngle - m_angle };

		// 区間(-π, π)に収める→diffの計算結果逆から回った方が早いときがあるからその時用
		while (diff > Math::Pi) diff -= Math::TwoPi;
		while (diff < -Math::Pi) diff += Math::TwoPi;

		double rotateSpeed{ 10.0 };		// 回転スピード
		m_angle += diff * rotateSpeed * deltaTime;

		this->SetRotation(Quaternion::RotateY(m_angle));
	}

	String nextKey = (m_move.lengthSq() > 0.1) ? U"Run" : U"Idle";
	anim->Play(nextKey, true);
}
