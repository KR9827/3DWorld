#pragma once
#include <Siv3D.hpp>
#include "GameObject.h"

class Camera : public GameObject
{
public:
	Camera(class SceneGame* game, std::shared_ptr<class Settings> settings);
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

	/// @brief 揺れを開始する
	/// @param duration 揺れの時間
	/// @param power 揺れの強さ
	void StartShake(double duration, double power);


private:
	std::shared_ptr<Settings> m_settings;

	std::weak_ptr<class GameObject> m_target;			// ターゲットの弱参照
	Vec3 m_offset{ 0.0, 5.0, -10.0 };					// ターゲットからカメラの位置をずらす変化量

	double m_radius{ 20.0f };

	double m_yaw{ static_cast<float>(-Math::Pi) / 2.0 };	// 左右回転
	double m_pitch{ 0.0 };									// 上下回転

	// 爆発の揺れ用
	double m_shakeTime{ 0.0 };
	double m_shakeDuration{ 0.0 };
	double m_shakePower{ 0.0 };
	Vec3 m_shakeOffset{ 0, 0, 0 };

	/// @brief カーソルが画面端に到達したら逆端に持っていく
	/// @return ture：画面端に到達
	bool WrapCursorAtWindowEdges();

};
