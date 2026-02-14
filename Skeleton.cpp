#include "Skeleton.h"
#include "SkinnedVertex.h"
#include "AnimationHelper.h"

void Skeleton::Initialize(const aiScene* scene)
{
	this->m_scene = scene;
	this->m_currentAnimScene = scene;			// 初期状態

	// FBXの構造をコンソールにすべて表示して名前を確認する
	//Console << U"--- FBX Node Hierarchy ---";
	//DisplayNodeHierarchy(m_scene->mRootNode, 0);
	//Console << U"--------------------------";

	// ルートノードの逆行列(GlobalInverse)をメッシュ側のルートから一度だけ計算
	m_globalInverseTransform = m_scene->mRootNode->mTransformation;
	m_globalInverseTransform.Inverse();

	CalculateBoneCount();

	SetAnimation(0);
	CalculateBoneTransform(0.0f);
}

void Skeleton::SetAnimation(int32 index)
{
	if (!m_scene || index >= static_cast<int32>(m_scene->mNumAnimations))
	{
		Console << U"Error：無効なアニメーションです";
		return;
	}

	m_currentAnimationIndex = index;
	m_currentTime = 0.0;
}

void Skeleton::SetAnimationScene(const aiScene* scene, int32 index)
{
	m_currentAnimScene = scene;
	m_currentAnimationIndex = index;
	m_currentTime = 0.0;

	aiMatrix4x4 globalInverse = m_currentAnimScene->mRootNode->mTransformation;
	globalInverse.Inverse();
	m_globalInverseTransform = globalInverse;
}

void Skeleton::CalculateBoneCount()
{
	std::unordered_set<std::string> boneName;
	for (uint32 i = 0; i < m_scene->mNumMeshes; ++i)
	{
		aiMesh* mesh{ m_scene->mMeshes[i] };
		for (uint32 j = 0; j < mesh->mNumBones; ++j)
		{
			aiBone* bone{ mesh->mBones[j] };
			boneName.insert(bone->mName.C_Str());
		}
	}
	int32 totalBoneCount{ static_cast<int32>(boneName.size()) };

	m_finalBoneTransform.resize(totalBoneCount);
}

void Skeleton::LoadBonesFromMesh(const aiMesh* mesh, Array<SkinnedVertex>& outVertices)
{
	outVertices.resize(mesh->mNumVertices);// 頂点の数だけ先に確保する

	for (uint32 i = 0; i < mesh->mNumBones; ++i)
	{
		std::string boneName{ mesh->mBones[i]->mName.C_Str() };

		// 登録されてないボーンを追加
		if (m_boneMapping.find(boneName) == m_boneMapping.end())
		{
			int32 newIndex{ static_cast<int32>(m_boneMapping.size()) };
			m_boneMapping[boneName] = newIndex;
			m_boneOffsetMatrices.resize(newIndex + 1);
		}
		m_boneOffsetMatrices[m_boneMapping[boneName]] = mesh->mBones[i]->mOffsetMatrix;

		int32 boneIndex{ m_boneMapping[boneName] };

		// ボーンが与える影響とそのウェイトを保存する
		aiBone* bone{ mesh->mBones[i] };
		for (uint32 j = 0; j < bone->mNumWeights; ++j)
		{
			const aiVertexWeight& weight{ bone->mWeights[j] };
			uint32 vertexId{ weight.mVertexId };
			float w{ weight.mWeight };

			outVertices[vertexId].boneIndices << boneIndex;
			outVertices[vertexId].boneWeight << w;
		}
	}

	for (auto& vertex : outVertices)
	{
		float totalWeight{ 0.0f };
		for (float& w : vertex.boneWeight)
		{
			totalWeight += w;
		}

		if (totalWeight > 0.0f)
		{
			for (float& w : vertex.boneWeight)
			{
				w /= totalWeight;
			}
		}
	}
}

void Skeleton::CalculateBoneTransform(float animationTime)
{
	// エラーチェック
	if (!m_currentAnimScene->HasAnimations())
	{
		Console << U"アニメーションが存在しません";
		return;
	}
	if (m_currentAnimationIndex < 0 || static_cast<uint32>(m_currentAnimationIndex) >= m_currentAnimScene->mNumAnimations)
	{
		Console << U"Error：m_currentAnimationIndexが範囲外です";
		return;
	}
	if (!m_currentAnimScene || !m_currentAnimScene->mAnimations) return;

	aiAnimation* anim = m_currentAnimScene->mAnimations[m_currentAnimationIndex];

	// 辿るルートノードはmeshのscene
	ReadNodeHierarchy(animationTime, m_currentAnimScene->mRootNode, aiMatrix4x4(), anim);
}

void Skeleton::CalvulateBoneTransform_StaticPose(int32 keyFrameIndex)
{

}

void Skeleton::ReadNodeHierarchy(float animationTime, const aiNode* node, const aiMatrix4x4& parentTransform, aiAnimation* animation)
{
	std::string nodeName{ node->mName.C_Str() };
	aiMatrix4x4 nodeTransform = node->mTransformation;

	// 補助ノードは変換を無視
	if (IsAssimpHelperNode(nodeName))
	{
		for (uint32 i = 0; i < node->mNumChildren; ++i)
		{
			ReadNodeHierarchy(animationTime, node->mChildren[i], parentTransform, animation);
		}
		return;
	}

	// アニメーションデータがあるか名前で探す
	aiNodeAnim* nodeAnim = FindNodeAnimation(animation, nodeName);

	if (nodeAnim)
	{
		// 位置、回転、スケールを補間する
		Vec3 position{ AnimationHelper::InterpolatePosition(nodeAnim, animationTime) };
		aiQuaternion rotation{ AnimationHelper::InterpolateRotation(nodeAnim, animationTime) };
		Vec3 scale{ AnimationHelper::InterpolateScale(nodeAnim, animationTime) };

		// アニメーションのその場化
		if (nodeName.find("Hips") != std::string::npos ||
			nodeName.find("Root") != std::string::npos)
		{
			position.x = 0.0f;
			position.z = 0.0f;
		}

		// 保険
		if (scale.x == 0) scale.x = 1;
		if (scale.y == 0) scale.y = 1;
		if (scale.z == 0) scale.z = 1;

		// 補間した位置、回転、スケールをaiMatrixに変換する
		aiMatrix4x4 translationMatrix, scalingMatrix;
		translationMatrix.Translation(aiVector3D(static_cast<float>(position.x), static_cast<float>(position.y), static_cast<float>(position.z)), translationMatrix);

		//aiMatrix4x4 rotationMatrix{ rotation.GetMatrix() };		// 回転：Quaternion -> Mat4x4 -> aiMatrix4x4
		aiMatrix4x4 rotationMatrix = aiMatrix4x4(rotation.GetMatrix());

		scalingMatrix.Scaling(aiVector3D(static_cast<float>(scale.x), static_cast<float>(scale.y), static_cast<float>(scale.z)), scalingMatrix);

		nodeTransform = translationMatrix * rotationMatrix * scalingMatrix;
	}

	aiMatrix4x4 globalTransform = parentTransform * nodeTransform;

	// ボーン行列を更新
	if (m_boneMapping.contains(nodeName))
	{
		uint32 boneIndex = m_boneMapping[nodeName];
		m_finalBoneTransform[boneIndex] = m_globalInverseTransform * globalTransform * m_boneOffsetMatrices[boneIndex];

	}

	// 子ノードに再帰
	for (uint32 i = 0; i < node->mNumChildren; ++i)
	{
		ReadNodeHierarchy(animationTime, node->mChildren[i], globalTransform, animation);
	}
}

void Skeleton::ReadNodeHierarchy_StaticPose(const aiNode* node, const aiMatrix4x4& parentTransform, aiAnimation* animation, int32 keyFrameIndex)
{
	std::string nodeName{ node->mName.C_Str() };

	// アニメーションチャンネルがある場合は補間する、ない場合はノードのデフォルトの変換を行う
	// nodeTransform：デフォルトのボーンの姿勢（BindPose）
	aiMatrix4x4 nodeTransform{ node->mTransformation };

	// チャンネル（アニメーション対象）を探す
	aiNodeAnim* channel{ FindNodeAnimation(animation, nodeName) };

	if (channel)
	{
		int32 posCount{ static_cast<int32>(channel->mNumPositionKeys) };
		int32 rotCount{ static_cast<int32>(channel->mNumRotationKeys) };
		int32 sclCount{ static_cast<int32>(channel->mNumScalingKeys) };

		int32 posIndex{ Min(keyFrameIndex, posCount - 1) };
		int32 rotIndex{ Min(keyFrameIndex, rotCount - 1) };
		int32 sclIndex{ Min(keyFrameIndex, sclCount - 1) };

		// 位置、回転、スケールを補完する
		aiVector3D position{ channel->mPositionKeys[posIndex].mValue };
		aiQuaternion rotation{ channel->mRotationKeys[rotIndex].mValue };
		aiVector3D scale{ channel->mScalingKeys[sclIndex].mValue };

		// 補完した位置、回転、スケールをaiMatrixに変換する
		// 位置
		aiMatrix4x4 translationMatrix;
		translationMatrix.Translation(position, translationMatrix);
		// 回転
		aiMatrix4x4 rotationMatrix{ rotation.GetMatrix() };
		// スケール
		aiMatrix4x4 scalingMatrix;
		scalingMatrix.Scaling(scale, scalingMatrix);

		nodeTransform = translationMatrix * rotationMatrix * scalingMatrix;
	}

	aiMatrix4x4 globalTransform{ parentTransform * nodeTransform };

	// このノードがボーンだった場合、m_finalBoneTransformを更新する
	auto it{ m_boneMapping.find(nodeName) };
	if (it != m_boneMapping.end())
	{
		int32 boneIndex{ it->second };
		m_finalBoneTransform[boneIndex] = m_globalInverseTransform * globalTransform * m_boneOffsetMatrices[boneIndex];
	}

	// 子ノードに再帰
	for (uint32 i = 0; i < node->mNumChildren; ++i)
	{
		ReadNodeHierarchy_StaticPose(node->mChildren[i], globalTransform, animation, keyFrameIndex);
	}
}

aiNodeAnim* Skeleton::FindNodeAnimation(aiAnimation* animation, const std::string& nodeName)
{
	for (uint32 i = 0; i < animation->mNumChannels; ++i)
	{
		aiNodeAnim* channel{ animation->mChannels[i] };
		if (nodeName == channel->mNodeName.C_Str())
		{
			return channel;
		}
	}

	return nullptr;
}

void Skeleton::DisplayNodeHierarchy(const aiNode* node, int32 depth)
{
	std::string indent(depth * 2, ' ');
	Console << Unicode::FromUTF8(indent + node->mName.C_Str());

	for (uint32 i = 0; i < node->mNumChildren; ++i)
	{
		DisplayNodeHierarchy(node->mChildren[i], depth + 1);
	}
}

void Skeleton::DisplayAnimationTime() const
{
	int32 currentAnimationIndex{ GetCurrentAnimationIndex() };
	aiAnimation* animation{ m_scene->mAnimations[currentAnimationIndex] };

	// アニメーション名
	String animationName;
	if (animation->mName.length > 0)
	{
		animationName = Unicode::FromUTF8(animation->mName.C_Str());
	}
	else
	{
		U"Animation_" + Format(currentAnimationIndex);
	}

	// アニメーションの全体時間
	double duration{ animation->mDuration / animation->mTicksPerSecond };

	Console << U"再生中のアニメーション：" << animationName;
	Console << U"現在の経過時間：" << m_currentTime << U"秒 / " << duration << U"秒";
}

bool Skeleton::IsAssimpHelperNode(const std::string& name)
{
	return name.find("$AssimpFbx$") != std::string::npos;
}
