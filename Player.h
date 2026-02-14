#pragma once
#include <Siv3D.hpp>
#include "GameObject.h"

class Player : public GameObject
{
public:
	Player(class SceneGame* game);
	~Player();

	void Initialize() override;

	/// @brief 更新処理
	/// @param deltaTime 前フレームからの経過時間
	void UpdateGameObject(float deltaTime) override;

	/// @brief 移動方向を渡す
	/// @return 移動方向
	Vec3 GetMove() const { return m_move; }

	Sphere GetAttackSphere() const { return m_Sphere; }
	Vec3 GetBonePos() const { return m_bonePos; }

private:
	static constexpr double MOVE_SPEED{ 20.0 };				// 移動速度

	class SceneGame* m_game;

	// コンボシステムのノード構造
	struct ComboNode
	{
		String animNode;						// このノードで再生するアニメ
		String boneName;						// このノードで攻撃判定を出すボーンの名前
		HashTable<char, ComboNode*> next;		// 次につながる入力
	};
	std::unique_ptr<ComboNode> m_root;
	ComboNode* m_currentNode{ nullptr };
	Array<std::unique_ptr<ComboNode>> m_nodes;

	Vec3 m_move{ 0.0, 0.0, 0.0 };				// プレイヤーの移動方向
	double m_angle{ 0.0 };						// プレイヤーの向いている方向
	int32 m_comboIndex{ 0 };					// 現在のコンボのインデックス

	bool m_hasHitThisAttack{ false };			// 攻撃がヒットしたかどうか（当たり判定）

	void CreateComboTree();
	void InputCombo(std::shared_ptr<class AnimationComponent> anim, char input);

	// 敵と自分の攻撃の当たり判定
	void CheckAttackCollision(std::shared_ptr<class AnimationComponent> anim);

	void ResolvePhysics();


	// 仮
	Sphere m_Sphere;
	Vec3 m_bonePos;

};
