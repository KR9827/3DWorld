#include "AnimationComponent.h"
#include "GameObject.h"
#include "Player.h"
#include "Skeleton.h"


AnimationComponent::AnimationComponent(std::weak_ptr<GameObject> owner, std::shared_ptr<class Skeleton> skeleton, int updateOrder)
	: Component(owner, updateOrder)
	, m_skeleton(skeleton)
{
	
}

AnimationComponent::~AnimationComponent()
{
	
}

void AnimationComponent::Initialize()
{
	m_animations[U"Idle"] = U"Assets/fbx/Idle.fbx";
	m_animations[U"Walk"] = U"Assets/fbx/Walking.fbx";
	m_animations[U"Run"] = U"Assets/fbx/Running.fbx";
	m_animations[U"Attack_L1"] = U"Assets/fbx/Punching.fbx";
	m_animations[U"Attack_L2"] = U"Assets/fbx/Punching2.fbx";
	m_animations[U"Attack_L3"] = U"Assets/fbx/Punching3.fbx";
	m_animations[U"Attack_R1"] = U"Assets/fbx/FrontKick.fbx";
	m_animations[U"Attack_R2"] = U"Assets/fbx/MmaKick.fbx";
	m_animations[U"Attack_R3"] = U"Assets/fbx/Martelo2.fbx";
	m_animations[U"Attack_R4"] = U"Assets/fbx/FlyingKick.fbx";
	m_animations[U"Attack_L1R1"] = U"Assets/fbx/HookPunch.fbx";
	m_animations[U"Attack_L2R1"] = U"Assets/fbx/JumpKick.fbx";
	m_animations[U"Attack_L3R1"] = U"Assets/fbx/InsideCrescentKick.fbx";
	
	
	

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
	if (m_currentAnimKey.isEmpty()) return;
	if (!m_playing) return;

	m_currentTime += deltaTime;

	if (m_loop)
	{
		if (m_duration > 0.0)
		{
			m_currentTime = fmod(m_currentTime, m_duration);
		}
	}
	else
	{
		if (m_currentTime >= m_duration)
		{
			m_currentTime = m_duration;
			m_playing = false;

			if (m_currentAnimKey.starts_with(U"Attack"))
			{
				m_currentAnimKey.clear();
			}
		}
	}

	m_skeleton->CalculateBoneTransform(static_cast<float>(m_currentTime * m_ticks));
}

void AnimationComponent::Play(const String& key, bool loop)
{
	if (m_currentAnimKey == key) return;

	if (!m_animations.contains(key) || !m_animationScenes.contains(key)) return;

	m_currentAnimKey = key;

	// Skeleton側に別FBXのシーンをセットする
	auto scene = m_animationScenes[key];

	m_loop = loop;
	m_playing = true;
	m_currentTime = 0.0;

	aiAnimation* anim = scene->mAnimations[0];
	m_ticks = anim->mTicksPerSecond != 0.0 ? anim->mTicksPerSecond : 25.0;
	m_duration = anim->mDuration / m_ticks;

	m_skeleton->SetAnimationScene(scene, 0);
}

void AnimationComponent::Stop()
{
	
}
