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
	void Play(const String& key);
	void Stop();

private:
	std::shared_ptr<class Skeleton> m_skeleton;

	HashTable<String, FilePath> m_animations;
	HashTable<String, const aiScene*> m_animationScenes;
	HashTable<String, std::unique_ptr<Assimp::Importer>> m_importers;

	String m_currentAnimKey;		// 現在再生中のアニメーションのキー
	double m_time{ 0.0 };

};
