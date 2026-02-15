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

	void RemoveGameObject(std::shared_ptr<class GameObject> gameObject) { m_gameObject.remove(gameObject); }

	std::shared_ptr<class Enemy> GetEnemy() const { return m_enemy; }
	std::shared_ptr<class Camera> GetCamera() const { return m_camera; }

private:
	Array<std::shared_ptr<class GameObject>> m_gameObject;					// ゲームオブジェクト
	Array<std::shared_ptr<class GameObject>> m_pendingGameObject;			// 保留中のゲームオブジェクト

	std::shared_ptr<class Player> m_player;
	std::shared_ptr<class Enemy> m_enemy;
	std::shared_ptr<class Camera> m_camera;
	std::shared_ptr<class Explosion> m_explosion;

	enum class GameState
	{
		WaitStart,
		Countdown,
		Playing,
		Exploding,
		Clear,
		Failed,

		Max,
	};

	GameState m_gameState;

	// 最初のカウントダウンと制限時間の変数
	double m_countdown;
	static constexpr double COUNTDOWN_TIME{ 3.0 };
	static constexpr double TIME_LIMIT{ 60.0 };
	Font m_waitStartFont;
	Font m_countdownFont;
	Font m_timeLimitFount;

	bool m_updatingGameObject;												// ゲームオブジェクトを更新しているかどうか

	ColorF m_backgroundColor;			// 背景色
	MSRenderTexture m_renderTexture;	// マルチサンプルレンダーテクスチャ

	Mesh m_groundMesh;					// 地面のメッシュ
	Texture m_groundTexture;			// 地面のテクスチャ
};
