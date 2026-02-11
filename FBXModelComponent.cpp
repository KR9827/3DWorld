#include "FBXModelComponent.h"
#include "SkinnedVertex.h"
#include "Skeleton.h"
#include "MeshWrapper.h"
#include "GameObject.h"

#include <filesystem>

FBXModelComponent::FBXModelComponent(std::weak_ptr<GameObject> owner, const FilePath& fbxFilePath, int updateOrder)
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
		// ボーン構造の生成(アニメーションがある場合)
		if (m_scene->HasAnimations())
		{
			m_skeleton = std::make_shared<Skeleton>();
			m_skeleton->Initialize(m_scene);
		}

		// メッシュの数だけ読み込む
		for (uint32 i = 0; i < m_scene->mNumMeshes; ++i)
		{
			aiMesh* mesh = m_scene->mMeshes[i];

			SubMesh sub;

			// メッシュデータをsiv3d用に変換
			MeshData data = ConvertToSiv3DMesh(mesh, sub);
			// MeshWrapperの生成
			sub.meshWrapper = std::make_unique<MeshWrapper>(data.vertices, data.indices);
			// テクスチャの読み込み
			sub.texture = LoadMaterialTexture(m_scene, mesh->mMaterialIndex, fbxFilePath);

			m_subMeshes << std::move(sub);
		}
	}
	else
	{
		Console << U"メッシュが存在しません";
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

void FBXModelComponent::Initialize()
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
	if (!m_isPlaying || !m_scene->HasAnimations() || !m_skeleton) return;

	auto& mats = m_skeleton->GetFinalBoneTransform();

	// スケルトンのボーン変形にしたがってスキニングをする(CPUで頂点変換する)
	for (auto& sub : m_subMeshes)
	{
		ApplySkinning(sub);
		UpdateMeshData(sub);
	}
}

void FBXModelComponent::Draw() const
{
	auto obj = m_owner.lock();
	if (!obj) return;

	for (const auto& sub : m_subMeshes)
	{
		if (sub.meshWrapper)
		{
			sub.meshWrapper->Draw(obj->GetWorldTransform(), sub.texture);
		}
	}
}

// fbx形式のモデルをsiv3dで扱えるMeshDataに変換する
MeshData FBXModelComponent::ConvertToSiv3DMesh(const aiMesh* mesh, SubMesh& sub)
{
	// 再ロードや更新時にデータが混ざらないように一回全部消す
	//m_vertices.clear();
	sub.indices.clear();
	sub.skinnedVertices.clear();

	// siv3dが描画に使う頂点構造体
	Vertex3D vertex{};
	Array<Vertex3D> vertices;
	vertices.clear();

	// 頂点の数
	uint32 vertexNum{ mesh->mNumVertices };
	// 頂点データの所得
	for (uint32 i = 0; i < vertexNum; ++i)
	{
		
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

		vertices << vertex;							// あとでGPUに送るデータ

		// スキニング用のデータにも格納する
		SkinnedVertex skinned{};					// アニメーション用の頂点
		skinned.originalPos = vertex.pos;			// スキニング前の位置を保存(毎フレーム変形するため)
		skinned.pos = vertex.pos;					// 最初は同じ値
		skinned.originalNormal = vertex.normal;		// スキニング前の法線を保存
		skinned.normal = vertex.normal;				// 変形後に再計算するため
		skinned.tex = vertex.tex;					// テクスチャ情報(描画に使用)

		sub.skinnedVertices << skinned;				// ボーン計算に使う
	}

	// インデックスデータの取得
	for (uint32 i = 0; i < mesh->mNumFaces; ++i)		// assimpがもってる面(face)の数
	{
		const aiFace& face = mesh->mFaces[i];		// aiFace：「何番の頂点を使うか」が入ってる

		if (face.mNumIndices == 3)		// 面が三角形の時
		{
			sub.indices << TriangleIndex32{ face.mIndices[0], face.mIndices[1], face.mIndices[2] };		// 頂点配列の何番をつないで三角形を作るかを指定
		}
	}

	if (m_skeleton)
	{
		m_skeleton->LoadBonesFromMesh(mesh, sub.skinnedVertices);
	}

	return MeshData{ vertices, sub.indices };
}

Texture FBXModelComponent::LoadMaterialTexture(const aiScene* scene, uint32 materialIndex, const FilePath& fbxFilePath)
{
	aiMaterial* material = scene->mMaterials[materialIndex];
	aiString path;
	aiColor4D color;

	// 基本色（デフューズテクスチャ）がある場合
	if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
	{
		const char* pathPtr = path.C_Str();

		// テクスチャが埋め込まれてる場合
		if (pathPtr[0] == '*')
		{
			int32 index = std::stoi(&pathPtr[1]);
			aiTexture* tex = scene->mTextures[index];

			// 圧縮データ(png/Jpg等)として読み込む
			if (tex->mHeight == 0)
			{
				return Texture{ MemoryReader{tex->pcData, tex->mWidth}, TextureDesc::Mipped };
			}
			// 非圧縮の場合
			else
			{

			}
		}
		// 外部ファイルの場合（fbxファイルと同じディレクトリにある）
		else
		{
			FilePath texturePath = FileSystem::ParentPath(fbxFilePath) + Unicode::FromUTF8(pathPtr);
			if (FileSystem::Exists(texturePath))
			{
				return Texture{ texturePath, TextureDesc::Mipped };
			}
		}
	}
	else if (aiGetMaterialColor(material, AI_MATKEY_BASE_COLOR, &color) == AI_SUCCESS ||
			 aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS)
	{
		return Texture{ Image{16, 16, ColorF{color.r, color.g, color.b, color.a}} };
	}

	// テクスチャがない、見つからない場合は白テクスチャを返す
	return Texture{ Image{16, 16, Palette::White} };
}

void FBXModelComponent::ApplySkinning(SubMesh& sub)
{

	// モデルを動かすために全ての頂点を1つずつ処理する
	for (size_t i = 0; i < sub.skinnedVertices.size(); ++i)
	{
		SkinnedVertex& vertex = sub.skinnedVertices[i];		// 直接書き換えるため、コピーではなく参照
		

		// ウェイトがないときは、元の位置をそのまま使う(頂点にボーンが割り当てられているか確認)
		if (vertex.boneWeight.empty())
		{
			vertex.pos = vertex.originalPos;
			vertex.normal = vertex.originalNormal;
		}
		else
		{
			Vec3 skinnedPos{ Vec3{0, 0, 0} };
			Vec3 skinnedNormal{ Vec3{0, 0, 0} };

			const auto& boneMatrices{ m_skeleton->GetFinalBoneTransform() };		// アニメーション後の骨の姿勢

			// 頂点に影響するボーンの数だけ回す
			for (size_t j = 0; j < vertex.boneIndices.size(); ++j)
			{
				int32 boneIndex{ vertex.boneIndices[j] };		// 何番目のボーンか
				float weight{ vertex.boneWeight[j] };			// そのボーンの影響率(0.0～1.0)

				const aiMatrix4x4 boneMatrix{ boneMatrices[boneIndex] };		// そのボーンの変形行列を取得(移動、回転、スケールが入っている)

				// 位置にスキニングを適用
				aiVector3D transformed{ boneMatrix * aiVector3D(static_cast<float>(vertex.originalPos.x), static_cast<float>(vertex.originalPos.y), static_cast<float>(vertex.originalPos.z)) };
				skinnedPos += Vec3{ transformed.x, transformed.y, transformed.z } *weight;		//複数ボーンの影響を足し合わせる、線形ブレンドスキニング(LBS)

				// 法線にスキニングを適用(回転・スケーリング部分のみ)
				aiMatrix3x3 normalMatrix{ aiMatrix3x3(boneMatrix) };
				normalMatrix.Inverse();
				normalMatrix.Transpose();
				aiVector3D transformedNormal{ normalMatrix * aiVector3D(static_cast<float>(vertex.originalNormal.x), static_cast<float>(vertex.originalNormal.y), static_cast<float>(vertex.originalNormal.z)) };
				skinnedNormal += Vec3{ transformedNormal.x, transformedNormal.y, transformedNormal.z } *weight;
			}

			vertex.pos = skinnedPos;
			vertex.normal = skinnedNormal.normalized();
		}
	}
}

void FBXModelComponent::UpdateMeshData(SubMesh& sub)
{
	Array<Vertex3D> skinnedMeshVertices;
	skinnedMeshVertices.reserve(sub.skinnedVertices.size());

	for (const auto& vertex : sub.skinnedVertices)
	{
		Vertex3D skinnedVertex;
		skinnedVertex.pos = vertex.pos;
		skinnedVertex.normal = vertex.normal;
		skinnedVertex.tex = vertex.tex;

		skinnedMeshVertices << skinnedVertex;
	}

	if (sub.meshWrapper)
	{
		sub.meshWrapper->UpdateMeshData(skinnedMeshVertices, sub.indices);
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
