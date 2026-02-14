#pragma once
#include <Siv3D.hpp>
#include <assimp/scene.h>
// 適宜ヘッダを追加

// このクラスの責務
// 　ボーン階層と変換行列の保持
// 　アニメーションの再生時間の管理
// 　指定された時刻のボーン行列(スキニング用)を計算
//   計算されたボーン行列はFBXModelComponentが取得してしようする

class Skeleton
{
public:
	/// @brief 初期化
	/// @param scene Assimpで読み込んだFBXモデルのシーン
	void Initialize(const aiScene* scene);

	/// @brief アニメーションを指定する
	/// @param index アニメーションインデックス
	void SetAnimation(int32 index);

	void SetAnimationScene(const aiScene* scene, int32 index);

	/// @brief アニメーションを更新
	/// @param deltaTime delta時間
	void UpdateAnimation(double deltaTime);

	/// @brief ボーンの変換行列を計算する
	/// @param animationTime アニメーションの再生時間
	void CalculateBoneTransform(float animationTime);


	/// @brief メッシュをもとにボーンのマッピングとウェイトの計算を行う
	/// @param mesh 対象のメッシュ
	/// @param outVertices 計算結果の保存先
	/// @note この関数はメッシュ作成後に呼び出す
	void LoadBonesFromMesh(const aiMesh* mesh, Array<struct SkinnedVertex>& outVertices);

	// == ヘッダー/ゲッター ==
	int32 GetCurrentAnimationIndex() const { return m_currentAnimationIndex; }
	const Array<aiMatrix4x4>& GetFinalBoneTransform() const { return m_finalBoneTransform; }
	void  SetGlobalinverseTransform(const aiMatrix4x4 matrix) { m_globalInverseTransform = matrix; }
	aiMatrix4x4 GetBoneGlobalTransform(const String& name) const;

private:
	const aiScene* m_scene{ nullptr };							// fbxモデルデータ
	const aiScene* m_currentAnimScene{ nullptr };				// アニメ側のデータの参照
	int32 m_currentAnimationIndex{ 0 };							// 現在のアニメーションのインデックス
	double m_currentTime{ 0.0 };								// アニメーションの現在の時間
	aiMatrix4x4 m_globalInverseTransform;						// シーン全体のルートノードのグローバル変換行列の逆行列
	HashTable<std::string, int32> m_boneMapping;				// ボーンマッピング用の連想配列
	Array<aiMatrix4x4> m_boneOffsetMatrices;					// オフセット行列
	HashTable<String, aiMatrix4x4> m_globalBoneTransforms;		// グローバル変換行列
	Array<aiMatrix4x4> m_finalBoneTransform;					// ボーンの最終変換行列

	// == 再生ループのための補助関数 ==

	// ボーンの数を計算してfinalBoneTransformを初期化する
	void CalculateBoneCount();

	/// @brief ボーンの変換行列を計算する
	/// @param animationTime アニメーションの再生時間
	void CalvulateBoneTransform_StaticPose(int32 keyFrameIndex);

	/// @brief 各ノードがボーンとして使われているときfinalBoneTransformを更新する
	/// @param animationTime アニメーションの再生時間
	/// @param node 対象のノード
	/// @param parentTransform 親の行列
	/// @param animation ボーンの行列
	void ReadNodeHierarchy(float animationTime, const aiNode* node, const aiMatrix4x4& parentTransform, aiAnimation* animation);
	void ReadNodeHierarchy_StaticPose(const aiNode* node, const aiMatrix4x4& parentTransform, aiAnimation* animation, int32 keyFrameIndex);

	/// @brief ノードのアニメーション（チャンネル）を検索する
	/// @param animation 対象のアニメーション
	/// @param nodeName ノード名
	/// @return 見つかる場合：対象のチャンネル、見つからない場合：nullptr
	aiNodeAnim* FindNodeAnimation(aiAnimation* animation, const std::string& nodeName);

	// == Debug情報 ==

	/// @brief [Debug] ノード情報を階層構造で表示する
	/// @param node ノード
	/// @param depth インデントの深さ（この値の分だけインデントがずれる）
	/// @note FBXモデルのノード情報（Assimpでいうところのチャンネル）を表示する
	void DisplayNodeHierarchy(const aiNode* node, int32 depth = 0);

	/// @brief [Debug] アニメーションの再生時間を表示する
	void DisplayAnimationTime() const;

	/// @brief $AssimpFbx$を除く
	/// @param name ノードの名前
	/// @return $AssimpFbx$が含まれてない時 true
	bool IsAssimpHelperNode(const std::string& name);
};
