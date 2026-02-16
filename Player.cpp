#include "Player.h"
#include "SceneGame.h"
#include "Camera.h"
#include "FBXModelComponent.h"
#include "AnimationComponent.h"
#include "SphereColliderComponent.h"
#include "Enemy.h"
#include "BoxColliderComponent.h"
#include "AudioManager.h"

Player::Player(SceneGame* game, std::shared_ptr<AudioManager> audio)
	: GameObject(game)
	, m_game(game)
	, m_audio(audio)
{
	CreateComboTree();
	m_currentNode = m_root.get();
}

Player::~Player()
{

}

void Player::Initialize()
{
	m_audio->PreLoadSE(U"Punchi", U"Assets/sounds/SE/Punchi.wav");
	m_audio->PreLoadSE(U"Kick", U"Assets/sounds/SE/Kick.wav");

	// コンポーネントを追加
	auto model = AddComponent<FBXModelComponent>(U"Assets/fbx/Walking.fbx", 100);
	AddComponent<AnimationComponent>(model->GetSkeleton(), 120);					// スケルトンのポインタをAnimationComponentに渡す
	AddComponent<SphereColliderComponent>(80);

	// コンポーネントの初期化
	auto comps = GetAllComponents();
	for (const auto& comp : comps)
	{
		comp->Initialize();
	}
}

void Player::UpdateGameObject(float deltaTime)
{
	// カメラを取得
	auto camera = m_game->GetCamera();
	if (!camera) return;

	// アニメーションコンポーネントを取得
	auto anim = GetComponent<AnimationComponent>();
	if (!anim) return;

	// ---------------------------
	// 攻撃
	// ---------------------------
	if (MouseL.down()) InputCombo(anim, 'L');
	if (MouseR.down()) InputCombo(anim, 'R');

	CheckAttackCollision(anim);

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

		// 移動で敵にめり込んでいたら戻す
		ResolvePhysics();

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
	auto L1 = std::make_unique<ComboNode>(ComboNode{ U"Attack_L1", U"mixamorig:LeftHand", U"P", 10, {}});
	auto L2 = std::make_unique<ComboNode>(ComboNode{ U"Attack_L2", U"mixamorig:RightHand", U"P", 20, {} });
	auto L3 = std::make_unique<ComboNode>(ComboNode{ U"Attack_L3", U"mixamorig:RightHand", U"P", 30, {} });
	auto R1 = std::make_unique<ComboNode>(ComboNode{ U"Attack_R1", U"mixamorig:LeftFoot", U"K", 20, {} });
	auto R2 = std::make_unique<ComboNode>(ComboNode{ U"Attack_R2", U"mixamorig:RightFoot", U"K", 40, {} });
	auto R3 = std::make_unique<ComboNode>(ComboNode{ U"Attack_R3", U"mixamorig:LeftFoot", U"K", 40, {} });
	auto R4 = std::make_unique<ComboNode>(ComboNode{ U"Attack_R4", U"mixamorig:LeftFoot", U"K", 50, {} });
	auto L1R1 = std::make_unique<ComboNode>(ComboNode{ U"Attack_L1R1", U"mixamorig:RightHand", U"P", 30, {} });
	auto L2R1 = std::make_unique<ComboNode>(ComboNode{ U"Attack_L2R1", U"mixamorig:LeftFoot", U"K", 50, {} });
	auto L3R1 = std::make_unique<ComboNode>(ComboNode{ U"Attack_L3R1", U"mixamorig:RightFoot", U"K", 60, {} });

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

		m_hasHitThisAttack = false;
	}
}

void Player::CheckAttackCollision(std::shared_ptr<AnimationComponent> anim)
{
	if (m_hasHitThisAttack) return;					// この攻撃がすでに当たっていたら読まない

	if (!anim->IsPlayingAttack()) return;			// 攻撃してないときは当たり判定をとらない

	// 今のノードの攻撃判定を出すボーンの名前と攻撃力を取得
	String boneName = m_currentNode->boneName;
	int32 damage = m_currentNode->damage;

	auto modelComp = GetComponent<FBXModelComponent>();
	auto enemy = m_game->GetEnemy();
	if (!enemy) return;

	// ボーンの位置をワールド座標で取得
	Mat4x4 boneMatrix = modelComp->GetBoneTransform(boneName);
	Mat4x4 worldMatrix = boneMatrix * GetWorldTransform();

	// ボーンの位置を球の中心にする
	Vec3 bonePos = worldMatrix.transformPoint(Vec3::Zero());
	Sphere attackSphere{ bonePos, 1.0 };						// 攻撃判定用の球
	

	// 敵の当たり判定用ボックスと交差してる時
	auto enemyCollider = enemy->GetComponent<BoxColliderComponent>();
	auto camera = m_game->GetCamera();
	if (enemyCollider && attackSphere.intersects(enemyCollider->GetBox()))
	{
		m_hasHitThisAttack = true;
		enemy->TakeDamage(damage);

		// SE
		if (m_currentNode->attackType == U"P")
		{
			m_audio->PlaySEPan(U"Punchi", enemy->GetPosition(), *camera);
		}
		else if (m_currentNode->attackType == U"K")
		{
			m_audio->PlaySEPan(U"Kick", enemy->GetPosition(), *camera);
		}
	}
}

void Player::ResolvePhysics()
{
	auto enemy = m_game->GetEnemy();
	if (!enemy) return;

	auto myCollider = GetComponent<SphereColliderComponent>();
	auto enemyCollider = enemy->GetComponent<BoxColliderComponent>();
	if (!myCollider || !enemyCollider) return;

	Sphere playerSphere = myCollider->GetSphere();
	Box enemyBox = enemyCollider->GetBox();

	if (playerSphere.intersects(enemyBox))
	{
		// ボックスから球の中心に最も近い点を求める
		Vec3 closestPoint{
			Max(enemyBox.center.x - enemyBox.size.x / 2.0, Min(playerSphere.center.x, enemyBox.center.x + enemyBox.size.x / 2.0)),
			Max(enemyBox.center.y - enemyBox.size.y / 2.0, Min(playerSphere.center.y, enemyBox.center.y + enemyBox.size.y / 2.0)),
			Max(enemyBox.center.z - enemyBox.size.z / 2.0, Min(playerSphere.center.z, enemyBox.center.z + enemyBox.size.z / 2.0))
		};

		// 球の中心とその点の距離を求める
		Vec3 delta = playerSphere.center - closestPoint;
		double dist = delta.length();

		// 球の半径より短いときは、その分だけ戻す
		if (dist < playerSphere.r)
		{
			Vec3 pushDir;
			double pushDist;

			// 球の中心とボックスの端が重なった時
			if (dist < 0.0001)
			{
				pushDir = Vec3{ 0, 0, 1.0 };				// 適当な方向に押す	
				pushDist = playerSphere.r;
			}
			else
			{
				pushDir = delta / dist;						// 押し戻す方向（正規化）
				pushDist = playerSphere.r - dist;			// 押し戻す距離
			}

			pushDir.y = 0;									// Y軸方向は押し戻さない
			SetPosition(GetPosition() + pushDir * pushDist);
		}
	}
}
