#include "FBXModelComponent.h"
#include "SkinnedVertex.h"
#include "Skeleton.h"
#include "MeshWrapper.h"
#include "GameObject.h"

#include <filesystem>

FBXModelComponent::FBXModelComponent(std::weak_ptr<GameObject> owner, const FilePath& fbxFilePath, const Optional<FilePath>& texFilePath, int updateOrder)
	: Component(owner, updateOrder)
	, m_scene(nullptr)
{
	// importerを破棄するとsceneも破棄されるのでメンバ変数にする

	// FBXファイルを読み込む
	// アニメーションの有無でフラグを変えるので、一旦tempSceneを読み込んでアニメーションの有無を確認する
	// その後、再度m_sceneに読み込む
	m_scene = m_importer.ReadFile(fbxFilePath.narrow(),
		aiProcess_Triangulate						// ポリゴンを全て三角形にする
		| aiProcess_JoinIdenticalVertices			// 同じ頂点をまとめる
		| aiProcess_LimitBoneWeights				// ボーンウェイトの数を4つまでに制限
		| aiProcess_ImproveCacheLocality			// GPU向け最適化
		| aiProcess_FlipUVs							// UV上下を反転
		| aiProcess_GenNormals
		| aiProcess_PopulateArmatureData			// ボーン構造を整理
		| aiProcess_OptimizeGraph
		//| aiProcess_ConvertToLeftHanded			// 左手座標系に変換
		//| aiProcess_MakeLeftHanded					// 左手座標系に強制
		| aiProcess_FlipWindingOrder				// 三角形の裏表を合わせる
		| aiProcess_GlobalScale						// スケールを自動調整
	);



	// ファイルが存在しない、またはルートノードがない（データが空）場合
	if (!m_scene || !m_scene->mRootNode)
	{
		throw std::runtime_error("FBXファイルのロードに失敗しました。理由：" + std::string(m_importer.GetErrorString()));
	}

	if (!m_scene || m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		Console << U"Assimp読み込みエラー：" << Unicode::FromUTF8(m_importer.GetErrorString());
		return;
	}

	// メッシュの生成
	if (m_scene->mNumMeshes > 0 && m_scene->mMeshes[0])
	{
		// ボーン構造の生成
		if (m_scene->HasAnimations())
		{
			m_skeleton = std::make_unique<Skeleton>();
			m_skeleton->Initialize(m_scene);
		}

		// Assimp -> siv3d meshDataに変換
		MeshData data{ ConvertToSiv3DMesh(m_scene->mMeshes[0]) };
		m_meshWrapper = std::make_unique<MeshWrapper>(data.vertices, data.indices);


		// boneTransform.clear();
	}
	else
	{
		Console << U"メッシュが存在しません";
	}

	// テクスチャの読み込み(必要なら)
	if (texFilePath)
	{
		LoadTexture(*texFilePath);
	}

	if (m_scene->HasAnimations())
	{
		m_isPlaying = true;
	}


}

FBXModelComponent::~FBXModelComponent()
{
	Console << U"FBXModelComponentが破棄されました";
}

void FBXModelComponent::Start()
{
	// FBXModelComponentを介して情報を渡すユーティリティ用処理

	// AABBの情報をGameObject側に渡す
	//const ModelAABBInfo aabb = ModelAABBUtil::ComputeAABBInfoFromScene(m_scene);
	//if (auto obj = m_owner.lock())
	//{
	//	obj->SetModelAABBInfo(aabb);
	//}
}

/// @brief モデルの更新処理
/// @note ボーンの行列を更新->各頂点にスキニングを適用->結果をメッシュに反映する
void FBXModelComponent::Update(double deltaTime)
{
	if (!m_isPlaying || !m_scene->HasAnimations())
	{
		return;
	}

	// アニメーションの更新
	m_skeleton->UpdateAnimation(deltaTime);

	// スケルトンのボーン変形にしたがってスキニングをする(CPUで頂点変換する)
	ApplySkinning();
	UpdateMeshData();
}

void FBXModelComponent::Draw() const
{
	if (!m_meshWrapper)
	{
		return;
	}

	if (auto obj = m_owner.lock())
	{
		m_meshWrapper->Draw(obj->GetWorldTransform(), m_texture);
	}
}

// fbx形式のモデルをsiv3dで扱えるMeshDataに変換する
MeshData FBXModelComponent::ConvertToSiv3DMesh(const aiMesh* mesh)
{
	// 再ロードや更新時にデータが混ざらないように一回全部消す
	m_vertices.clear();
	m_indices.clear();
	m_skinnedVertices.clear();

	// 頂点の数
	uint32 vertexNum{ mesh->mNumVertices };
	// 頂点データの所得
	for (uint32 i = 0; i < vertexNum; ++i)
	{
		Vertex3D vertex{};		// siv3dが描画に使う頂点構造体
		// assimp座標からsiv3dの座標へ変換
		vertex.pos = Vec3{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

		// 法線(ライティング計算に法線が必要)
		vertex.normal = mesh->HasNormals()
			? Vec3{ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z }
			: Vec3{ 0.0, 1.0, 0.0 };															// ない場合はデフォルト設定

		// UV(テクスチャ座標があるか確認)
		vertex.tex = mesh->HasTextureCoords(0)
			? Vec2{ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y }
			: Vec2{ 0.0, 0.0 };																	// ない場合はデフォルト設定
		
		m_vertices << vertex;		// あとでGPUに送るデータ

		// スキニング用のデータにも格納する
		SkinnedVertex skinned{};					// アニメーション用の頂点
		skinned.originalPos = vertex.pos;		// スキニング前の位置を保存(毎フレーム変形するため)
		skinned.pos = vertex.pos;				// 最初は同じ値
		skinned.normal = vertex.normal;			// 変形後に再計算するため
		skinned.tex = vertex.tex;				// テクスチャ情報(描画に使用)

		m_skinnedVertices << skinned;		// ボーン計算に使う

	}

	// インデックスデータの取得
	for (uint32 i = 0; i < mesh->mNumFaces; ++i)		// assimpがもってる面(face)の数
	{
		const aiFace& face = mesh->mFaces[i];		// aiFace：「何番の頂点を使うか」が入ってる

		if (face.mNumIndices == 3)		// 面が三角形の時
		{
			m_indices << TriangleIndex32{ face.mIndices[0], face.mIndices[1], face.mIndices[2] };		// 頂点配列の何番をつないで三角形を作るかを指定
		}
	}

	if (m_skeleton)
	{
		m_skeleton->LoadBonesFromMesh(mesh, m_skinnedVertices);
	}
	MeshData meshData{ MeshData(m_vertices, m_indices) };

	return meshData;
}

void FBXModelComponent::LoadTexture(const FilePath& texturePath)
{
	m_texture = Texture(texturePath);
}

void FBXModelComponent::ApplySkinning()
{

	// モデルを動かすために全ての頂点を1つずつ処理する
	for (size_t i = 0; i < m_skinnedVertices.size(); ++i)
	{		
		SkinnedVertex& vertex = m_skinnedVertices[i];		// 直接書き換えるため、コピーではなく参照

		// ウェイトがないときは、元の位置をそのまま使う(頂点にボーンが割り当てられているか確認)
		if (vertex.boneWeight.empty())
		{
			vertex.pos = vertex.originalPos;
			vertex.normal = vertex.normal.normalized();
		}
		else
		{
			Vec3 skinnedPos{ Vec3{0, 0, 0} };
			Vec3 skinnedNormal{ Vec3{0, 0, 0} };

			const auto& boneMatrices{ m_skeleton->GetFinalBoneTransform()};		// アニメーション後の骨の姿勢

			// 頂点に影響するボーンの数だけ回す
			for (size_t j = 0; j < vertex.boneIndices.size(); ++j)
			{
				int32 boneIndex{ vertex.boneIndices[j] };		// 何番目のボーンか
				float weight{ vertex.boneWeight[j] };			// そのボーンの影響率(0.0～1.0)
			
			
				
				const aiMatrix4x4 boneMatrix{ boneMatrices[boneIndex] };		// そのボーンの変形行列を取得(移動、回転、スケールが入っている)
			
				// 位置にスキニングを適用
				aiVector3D transformed{ boneMatrix * aiVector3D(static_cast<float>(vertex.originalPos.x), static_cast<float>(vertex.originalPos.y), static_cast<float>(vertex.originalPos.z)) };
				skinnedPos += Vec3{ transformed.x, transformed.y, transformed.z } * weight;		//複数ボーンの影響を足し合わせる、線形ブレンドスキニング(LBS)
			
				// 法線にスキニングを適用(回転・スケーリング部分のみ)
				aiMatrix3x3 normalMatrix{ aiMatrix3x3(boneMatrix) };
				aiVector3D transformedNormal{ normalMatrix * aiVector3D(static_cast<float>(vertex.normal.x), static_cast<float>(vertex.normal.y), static_cast<float>(vertex.normal.z)) };
				skinnedNormal += Vec3{ transformedNormal.x, transformedNormal.y, transformedNormal.z } * weight;
			}

			vertex.pos = skinnedPos;
			vertex.normal = skinnedNormal.normalized();
		}
	}
}

void FBXModelComponent::UpdateMeshData()
{
	Array<Vertex3D> skinnedMeshVertices;
	skinnedMeshVertices.reserve(m_skinnedVertices.size());

	for (const auto& vertex : m_skinnedVertices)
	{
		Vertex3D skinnedVertex;
		skinnedVertex.pos = vertex.pos;
		skinnedVertex.normal = vertex.normal;
		skinnedVertex.tex = vertex.tex;

		skinnedMeshVertices << skinnedVertex;
	}

	if (m_meshWrapper)
	{
		m_meshWrapper->UpdateMeshData(skinnedMeshVertices, m_indices);
	}
}

void FBXModelComponent::StartAnimation()
{
	m_isPlaying = true;
}

void FBXModelComponent::StopAnimation()
{
	m_isPlaying = false;
}
