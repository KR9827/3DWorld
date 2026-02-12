#pragma once
#include <Siv3D.hpp>
#include "Component.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class AnimationComponent : public Component
{
public:
	AnimationComponent(std::weak_ptr<class GameObject> owner, std::shared_ptr<class Skeleton> skeleton, int updateOrder = 100);
	~AnimationComponent();

	void Initialize() override;
	void Update(double deltaTime) override;

	/// @brief アニメーションを再生する
	/// @param name 再生するアニメーションの名前
	void Play(const String& key, bool loop = true);
	void Stop();

	bool IsPlayingAttack() const { return m_playing && m_currentAnimKey.starts_with(U"Attack"); }
	bool IsFinishedAttack() const { return !m_playing && m_currentAnimKey.starts_with(U"Attack"); }
	double GetRemainingTime() const { return m_duration - m_currentTime; }

private:
	std::shared_ptr<class Skeleton> m_skeleton;

	HashTable<String, FilePath> m_animations;
	HashTable<String, const aiScene*> m_animationScenes;
	HashTable<String, std::unique_ptr<Assimp::Importer>> m_importers;

	String m_currentAnimKey;		// 現在再生中のアニメーションのキー
	double m_currentTime{ 0.0 };
	double m_ticks{ 0.0 };
	double m_duration{ 0.0 };
	bool m_loop{ true };
	bool m_playing{ false };

};
