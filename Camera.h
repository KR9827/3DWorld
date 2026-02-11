#pragma once
#include <Siv3D.hpp>
#include "GameObject.h"

class Camera : public GameObject
{
public:
	Camera(class SceneGame* game);
	~Camera();

	/// @brief 更新処理
	/// @param deltaTime 前フレームからの経過時間
	void UpdateGameObject(float deltaTime) override;

	/// @brief 3D描画のカメラオブジェクトを生成して返す
	/// @return カメラオブジェクト
	BasicCamera3D GetCamera() const;

	/// @brief 追従する対象をセットする
	/// @param target 追従する対象のポインタ
	void SetTarget(std::shared_ptr<class GameObject> target) { m_target = target; }

	/// @brief 現在のカメラの向きの正面を取得
	/// @return カメラからみた正面のVec3を返す
	Vec3 GetForward() const;

	/// @brief 現在のカメラの向きの右方向を取得
	/// @return カメラから見た右方向のVec3を返す
	Vec3 GetRight() const;

private:
	std::weak_ptr<class GameObject> m_target;	// ターゲットの弱参照
	Vec3 m_offset{ 0.0, 5.0, -10.0 };					// ターゲットからカメラの位置をずらす変化量

	float m_angle{ -Math::Pi / 2.0f };
	float m_radius{ 20.0f };
};
