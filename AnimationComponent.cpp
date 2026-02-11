#include "AnimationComponent.h"
#include "GameObject.h"
#include "Player.h"
#include "Skeleton.h"


AnimationComponent::AnimationComponent(std::weak_ptr<GameObject> owner, std::shared_ptr<class Skeleton> skeleton, int updateOrder)
	: Component(owner, updateOrder)
	, m_skeleton(skeleton)
{
	Initialize();
}

AnimationComponent::~AnimationComponent()
{
	
}

void AnimationComponent::Initialize()
{
	m_animations[U"Idle"] = U"Assets/fbx/Idle.fbx";
	m_animations[U"Walk"] = U"Assets/fbx/Walking.fbx";
	m_animations[U"Run"] = U"Assets/fbx/Running.fbx";

	for (auto& [key, path] : m_animations)
	{
		auto importer = std::make_unique<Assimp::Importer>();

		const aiScene* scene = importer->ReadFile(path.narrow(),
			aiProcess_Triangulate
			| aiProcess_LimitBoneWeights
			| aiProcess_PopulateArmatureData
			| aiProcess_GlobalScale						// スケールを自動調整
		);

		if (!scene)
		{
			Console << U"Animation Load Failed: " << key;
			continue;
		}

		m_animationScenes[key] = scene;
		m_importers[key] = std::move(importer);
	}
}

void AnimationComponent::Update(double deltaTime)
{
	auto owner = GetOwner().lock();
	if (!owner) return;

	auto player = std::dynamic_pointer_cast<Player>(owner);

	if (player)
	{
		String nextKey = (player->GetMove().lengthSq() > 0.1) ? U"Run" : U"Idle";

		if (m_currentAnimKey != nextKey)
		{
			this->Play(nextKey);
		}
	}

	if (m_currentAnimKey.isEmpty()) return;

	m_time += deltaTime;

	m_skeleton->UpdateAnimation(deltaTime);
}

void AnimationComponent::Play(const String& key)
{
	if (m_currentAnimKey == key) return;

	if (!m_animations.contains(key) || !m_animationScenes.contains(key)) return;

	m_currentAnimKey = key;
	m_time = 0.0;

	// Skeleton側に別FBXのシーンをセットする
	auto scene = m_animationScenes[key];
	m_skeleton->SetAnimationScene(scene, 0);
}

void AnimationComponent::Stop()
{
	
}
