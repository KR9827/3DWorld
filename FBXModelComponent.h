#pragma once
#include <Siv3D.hpp>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Component.h"
// 必要なヘッダを適宜追加する

// このクラスの責務
// FBXファイルをsiv3dで扱い可能なデータへ変換して、Mesh情報を保持する
// Skeletonに対してアニメーション再生を指示
// Skeletonから取得したボーンの行列をもとにスキニング処理を行う
// メッシュデータを更新して描画する

// アニメーションやボーンの計算はSkeletonクラスに任せる
class FBXModelComponent : public Component
{
public:
	/// @brief コンストラクタ
	/// @param owner コンポーネントを付ける対象
	/// @param fbxFilePath 読み込むFBXファイルのパス
	/// @param texFilePath 読み込むテクスチャファイルのパス（デフォルト：なし）
	/// @param updateOrder コンポーネントを更新する時の優先度
	FBXModelComponent(std::weak_ptr<class GameObject> owner, const FilePath& fbxFilePath, const Optional<FilePath>& texFilePath = none, int updateOrder = 100);	// fbxモデルのパスとテクスチャのパス
	~FBXModelComponent();

	void Start() override;
	void Update(double deltaTime) override;
	void Draw() const override;

private:
	std::unique_ptr<class Skeleton> m_skeleton;
	std::unique_ptr<class MeshWrapper> m_meshWrapper;

	Array<Vertex3D> m_vertices;												// 頂点
	Array<TriangleIndex32> m_indices;										// インデックス配列
	Array<struct SkinnedVertex> m_skinnedVertices;							// スキニングデータ用

	Assimp::Importer m_importer;
	const aiScene* m_scene;													// FBXのシーンデータ


	DynamicMesh m_mesh;														// fbxのメッシュ
	Texture m_texture;														// 使用するテクスチャ

	HashTable<std::string, aiMatrix4x4> m_boneTransform;					// ボーンのノードと変換行列の連想配列

	bool m_isPlaying{ true };												// アニメーション再生のフラグ

	/// @brief fbx形式のモデルをsiv3dで扱えるMeshDataに変換する
	/// @param mesh Assimpで読み込んだメッシュ
	/// @return siv3dで使えるデータ型のメッシュ
	MeshData ConvertToSiv3DMesh(const aiMesh* mesh);

	/// @brief テクスチャのロード
	/// @param texturePath テクスチャのパス
	void LoadTexture(const FilePath& texturePath);

	/// @brief ボーンの変形行列を使って各頂点のアニメーション後の位置に変換する
	void ApplySkinning();

	/// @brief メッシュデータを更新する
	void UpdateMeshData();

	/// @brief アニメーションを再生する
	void StartAnimation();

	/// @brief アニメーションを止める
	void StopAnimation();
};
