#pragma once
#include <Siv3D.hpp>
#include "SceneController.h"

class SceneGame : public App::Scene
{
public:
	SceneGame(const InitData& init);
	~SceneGame();

	bool SystemInit();
	void GameInit();
	void update() override;
	void draw() const override;

	// GameObjectを追加するテンプレート関数
	template <class T = GameObject, class... Args>				// class... Arg：引数が何個でもあっていい
	std::shared_ptr<T> AddGameObject(Args&&... args)			// Args&&... atgs：対象の引数に自動展開される
	{
		// T型のGameObjectを生成
		auto obj = std::make_shared<T>(this, std::forward<Args>(args)...);			// forward～：引数の渡し方をそのまま保つ

		// リストに追加
		m_gameObject << obj;

		return obj;
	}

	void RemoveGameObject(std::shared_ptr<class GameObject> gameObject);

private:
	Array<std::shared_ptr<class GameObject>> m_gameObject;					// ゲームオブジェクト
	Array<std::shared_ptr<class GameObject>> m_pendingGameObject;			// 保留中のゲームオブジェクト

	std::shared_ptr<class Player> m_player;
	std::shared_ptr<class Camera> m_camera;

	bool m_updatingGameObject;												// ゲームオブジェクトを更新しているかどうか



	Sphere m_sphere;
};
