#include "Player.h"
#include "SceneGame.h"
#include "Camera.h"
#include "AnimationComponent.h"

Player::Player(SceneGame* game)
	: GameObject(game)
	, m_game(game)
{
	CreateComboTree();
	m_currentNode = m_root.get();
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
	if (MouseL.down()) InputCombo(anim, 'L');
	if (MouseR.down()) InputCombo(anim, 'R');

	if (anim->IsPlayingAttack()) return;


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

void Player::CreateComboTree()
{
	// アニメ無しの開始地点
	m_root = std::make_unique<ComboNode>(ComboNode{ U"", {} });

	// 各ノードの登録
	auto L1 = std::make_unique<ComboNode>(ComboNode{ U"Attack_L1", {} });
	auto L2 = std::make_unique<ComboNode>(ComboNode{ U"Attack_L2", {} });
	auto L3 = std::make_unique<ComboNode>(ComboNode{ U"Attack_L3", {} });
	auto R1 = std::make_unique<ComboNode>(ComboNode{ U"Attack_R1", {} });
	auto R2 = std::make_unique<ComboNode>(ComboNode{ U"Attack_R2", {} });
	auto R3 = std::make_unique<ComboNode>(ComboNode{ U"Attack_R3", {} });
	auto R4 = std::make_unique<ComboNode>(ComboNode{ U"Attack_R4", {} });
	auto L1R1 = std::make_unique<ComboNode>(ComboNode{ U"Attack_L1R1", {} });
	auto L2R1 = std::make_unique<ComboNode>(ComboNode{ U"Attack_L2R1", {} });
	auto L3R1 = std::make_unique<ComboNode>(ComboNode{ U"Attack_L3R1", {} });

	// コンボ登録
	L1->next['L'] = L2.get();
	L1->next['R'] = L1R1.get();
	L2->next['L'] = L3.get();
	L2->next['R'] = L2R1.get();
	L3->next['R'] = L3R1.get();
	R1->next['R'] = R2.get();
	R2->next['R'] = R3.get();
	R3->next['R'] = R4.get();
	m_root->next['L'] = L1.get();
	m_root->next['R'] = R1.get();

	// ノードに生ポインタを渡す
	m_nodes << std::move(L1);
	m_nodes << std::move(L2);
	m_nodes << std::move(L3);
	m_nodes << std::move(R1);
	m_nodes << std::move(R2);
	m_nodes << std::move(R3);
	m_nodes << std::move(R4);
	m_nodes << std::move(L1R1);
	m_nodes << std::move(L2R1);
	m_nodes << std::move(L3R1);
}

void Player::InputCombo(std::shared_ptr<AnimationComponent> anim, char input)
{
	// 攻撃してない時、必ず初めからスタート
	if (!anim->IsPlayingAttack())
	{
		m_currentNode = m_root.get();
	}
	else
	{
		// 受付時間外は入力を無効
		const float inputTime = 0.2f;
		if (anim->GetRemainingTime() > inputTime) return;
	}

	// 今のノードから派生するノードがある時
	if (m_currentNode->next.contains(input))
	{
		m_currentNode = m_currentNode->next[input];				// ノードを次に進める
		anim->Play(m_currentNode->animNode, false);				// そのノードのアニメを再生
	}
}
