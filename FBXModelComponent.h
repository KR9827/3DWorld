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
	FBXModelComponent(std::weak_ptr<class GameObject> owner, const FilePath& fbxFilePath, int updateOrder = 100);	// fbxモデルのパスとテクスチャのパス
	~FBXModelComponent();

	void Initialize() override;
	void Update(double deltaTime) override;
	void Draw() const override;

	Mat4x4 GetBoneTransform(const String& boneName) const;
	std::shared_ptr<class Skeleton> GetSkeleton() const { return m_skeleton; }

private:
	/// @brief fbxファイル内のメッシュとテクスチャを保持する
	struct SubMesh
	{
		std::unique_ptr<class MeshWrapper> meshWrapper;
		Array<struct SkinnedVertex> skinnedVertices;			// スキニングデータ
		Array<TriangleIndex32> indices;							// インデックス
		Texture texture;										// テクスチャ
	};

	std::shared_ptr<class Skeleton> m_skeleton;

	Array<SubMesh> m_subMeshes;												// 複数のメッシュやテクスチャを保持する配列

	Assimp::Importer m_importer;
	const aiScene* m_scene;													// FBXのシーンデータ


	DynamicMesh m_mesh;														// fbxのメッシュ
	Texture m_texture;														// 使用するテクスチャ

	HashTable<std::string, aiMatrix4x4> m_boneTransform;					// ボーンのノードと変換行列の連想配列

	bool m_isPlaying{ true };												// アニメーション再生のフラグ

	/// @brief fbx形式のモデルをsiv3dで扱えるMeshDataに変換する
	/// @param mesh Assimpで読み込んだメッシュ
	/// @return siv3dで使えるデータ型のメッシュ
	MeshData ConvertToSiv3DMesh(const aiMesh* mesh, SubMesh& sub);

	/// @brief マテリアルからテクスチャをロード
	/// @param texturePath テクスチャのパス
	Texture LoadMaterialTexture(const aiScene* scene, uint32 materialIndex, const FilePath& fbxFilePath);

	/// @brief ボーンの変形行列を使って各頂点のアニメーション後の位置に変換する
	void ApplySkinning(SubMesh& sub);

	/// @brief メッシュデータを更新する
	void UpdateMeshData(SubMesh& sub);

	/// @brief アニメーションを再生する
	void StartAnimation();

	/// @brief アニメーションを止める
	void StopAnimation();
};
