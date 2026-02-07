#pragma once
#include <Siv3D.hpp>
#include <assimp/scene.h>

namespace AnimationHelper
{
	/// @brief 現在のアニメーション時間に対して、どの2つのキーフレームを使うかを探す(位置、スケール用)
	/// @param currentTime 現在の時間
	/// @param keys キーフレーム一覧
	/// @param numKeys キーフレーム総数
	/// @return 特定したキーのインデックス
	int32 FindKeyIndex(float currentTime, const aiVectorKey* keys, int32 numKeys);

	/// @brief 現在のアニメーション時間に対して、どの2つのキーフレームを使うかを探す(回転用)
	/// @param currentTime 現在の時間
	/// @param keys キーフレーム一覧
	/// @param numKeys キーフレーム総数
	/// @return 特定したキーのインデックス
	int32 FindKeyIndex(float currentTime, const aiQuatKey* keys, int32 numKeys);

	/// @brief 補間係数を計算して取得
	/// @param currentTime 現在の時間
	/// @param startTime スタート時間
	/// @param endTime 終了時間
	/// @return 係数
	float GetInterpolationFactor(float currentTime, float startTime, float endTime);

	/// @brief 位置の補間
	/// @param channel 対象のアニメーションのチャンネル
	/// @param animationTime 対象アニメーションの時間
	/// @return 補間によって算出した位置
	Vec3 InterpolatePosition(aiNodeAnim* channel, float animationTime);

	/// @brief 回転の補間
	/// @param channel 対象のアニメーションのチャンネル
	/// @param animationTime 対象アニメーションの時間
	/// @return 補間によって算出したクォータニオン
	aiQuaternion InterpolateRotation(aiNodeAnim* channel, float animationTime);

	/// @brief スケールの補間
	/// @param channel 対象のアニメーションのチャンネル
	/// @param animationTime 対象アニメーションの時間
	/// @return 補間によって算出したスケール
	Vec3 InterpolateScale(aiNodeAnim* channel, float animationTime);

	/// @brief 線形補間
	/// @param start 初期座標
	/// @param end 終了座標
	/// @param factor 補間係数
	/// @return 線形補間の結果
	Vec3 Lerp(const Vec3& start, const Vec3& end, float factor);

	/// @brief 球面線形補間
	/// @param q1 値1
	/// @param q2 値2
	/// @param factor 補間係数 
	/// @return 球面線形補間の結果
	/// @attention 現在使ってないので使用する際にはこの関数の中身を確認してください
	Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float factor);

	/// @brief aiMatrixの行列を表示
	/// @param m aiMatrix型の行列
	/// @param label 行列の上に表示する文字列ラベル
	void PrintAiMatrix(const aiMatrix4x4& m, const String& label);
}
