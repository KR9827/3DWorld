#pragma once
#include <Siv3D.hpp>
#include "GameObject.h"

class Player : public GameObject
{
public:
	Player(class SceneGame* game);
	~Player();

	/// @brief 更新処理
	/// @param deltaTime 前フレームからの経過時間
	void UpdateGameObject(float deltaTime) override;

	/// @brief 移動方向を渡す
	/// @return 移動方向
	Vec3 GetMove() const { return m_move; }

private:
	static constexpr double MOVE_SPEED{ 20.0 };				// 移動速度

	class SceneGame* m_game;

	struct ComboNode
	{
		String animNode;						// このノードで再生するアニメ
		HashTable<char, ComboNode*> next;		// 次につながる入力
	};
	std::unique_ptr<ComboNode> m_root;
	ComboNode* m_currentNode{ nullptr };
	Array<std::unique_ptr<ComboNode>> m_nodes;

	Vec3 m_move{ 0.0, 0.0, 0.0 };
	double m_angle{ 0.0 };

	int32 m_comboIndex{ 0 };
	bool m_comboQueued{ false };

	void CreateComboTree();
	void InputCombo(std::shared_ptr<class AnimationComponent> anim, char input);

};
