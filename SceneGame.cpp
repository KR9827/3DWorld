#include "SceneGame.h"
#include "GameObject.h"
#include "Player.h"
#include "Enemy.h"
#include "Camera.h"
#include "Explosion.h"

SceneGame::SceneGame(const InitData& init)
	: IScene(init)
{
	SystemInit();
	GameInit();
}

SceneGame::~SceneGame()
{

}

bool SceneGame::SystemInit()
{
	m_backgroundColor = ColorF{ Palette::Dodgerblue };
	Scene::SetBackground(m_backgroundColor);

	m_renderTexture = MSRenderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };

	// 光源の設定
	Graphics3D::SetGlobalAmbientColor(ColorF{ 0.75, 0.75, 0.75 });			// 環境光の設定
	Graphics3D::SetSunDirection(Vec3{ 0.3, 10.0, 0.2 });					// 太陽の方向
	Graphics3D::SetSunColor(ColorF{ 1.0 });									// 太陽光の設定

	m_groundMesh = Mesh{ MeshData::OneSidedPlane(2000, {400, 400}) };
	m_groundTexture = Texture{ U"example/texture/ground.jpg", TextureDesc::MippedSRGB };

	// プレイヤーの生成
	m_player = AddGameObject<Player>();
	m_player->SetPosition(Vec3{ 0, 0, 0});
	m_player->SetScale(3.0f);
	m_player->SetRotation(Quaternion::Identity());

	// カメラ生成
	m_camera = AddGameObject<Camera>();
	m_camera->SetTarget(m_player);			// 追従対象を指定
	m_camera->SetPosition(Vec3{ 0, 5, -10 });

	// 敵の生成
	m_enemy = AddGameObject<Enemy>();
	m_enemy->SetPosition(Vec3{ 0.0, 0.0, 15.0 });

	m_gameState = GameState::WaitStart;
	m_countdown = 0.0;

	// フォントのパス設定
	const FilePath path = (FileSystem::GetFolderPath(SpecialFolder::SystemFonts) + U"HGRSGU.TTC");		// Windows内のフォント
	// フォントの生成
	m_waitStartFont = Font{ FontMethod::MSDF, 80, path };
	m_countdownFont = Font{ FontMethod::MSDF, 80, path };
	m_timeLimitFount = Font{ FontMethod::MSDF, 60, path };

	for (const auto& obj : m_gameObject)
	{
		obj->Initialize();
	}

	return true;
}

void SceneGame::GameInit()
{

}

void SceneGame::update()
{
	ClearPrint();

	const double dt = Scene::DeltaTime();
	auto& data = getData();

	// ゲームの状態遷移
	switch (m_gameState)
	{
	case GameState::WaitStart:
		if (MouseL.down())
		{
			m_countdown = COUNTDOWN_TIME;
			m_gameState = GameState::Countdown;
		}
		break;

	case GameState::Countdown:
		m_countdown -= dt;
		if (m_countdown <= -1.0)
		{
			m_countdown = TIME_LIMIT;
			m_gameState = GameState::Playing;
		}
		break;

	case GameState::Playing:
		m_countdown -= dt;

		// ゲームオブジェクトの更新
		for (auto& obj : m_gameObject)
		{
			obj->Update(static_cast<float>(dt));
		}

		// 成功か失敗の判定
		if (m_enemy && m_enemy->GetHP() <= 0)
		{
			// 爆発エフェクトの生成
			m_explosion = AddGameObject<Explosion>();
			m_explosion->SetPosition(m_enemy->GetPosition());
			m_explosion->Initialize();

			RemoveGameObject(m_enemy);
			m_enemy.reset();

			m_camera->StartShake(1.0, 2.0);		// カメラを揺らす

			m_gameState = GameState::Exploding;
		}
		else if (m_countdown <= 0.0)
		{
			m_gameState = GameState::Failed;
		}

		break;

	case GameState::Exploding:
		m_camera->UpdateGameObject(dt);
		if (m_explosion)
		{
			m_explosion->Update(static_cast<float>(dt));

			if (m_explosion->isFinished())
			{
				RemoveGameObject(m_explosion);
				m_explosion.reset();
				m_gameState = GameState::Clear;
			}
		}

		break;

	case GameState::Clear:
		Print << U"success!";
		data.isClear = true;
		changeScene(SceneState::RESULT);
		break;

	case GameState::Failed:
		Print << U"Failed...";
		data.isClear = false;
		changeScene(SceneState::RESULT);
		break;
	}

	if (KeyEnter.down())
	{
		changeScene(SceneState::RESULT);
	}
}

void SceneGame::draw() const
{
	if (!m_camera) return;

	//--------------------------
	// 3D描画
	// -------------------------
	{
		{
			const ScopedRenderTarget3D target{ m_renderTexture.clear(m_backgroundColor) };

			// カメラの行列
			const BasicCamera3D camera{ m_camera->GetCamera() };			// カメラを生成
			Graphics3D::SetCameraTransform(camera);


			// 座標軸を表示する（赤：X, 緑：Y, 青：Z）
			//for (int i = 0; i < 10; ++i) Line3D{ Vec3{0,0,0}, Vec3{i,0,0} }.draw(Palette::Red);
			//for (int i = 0; i < 10; ++i) Line3D{ Vec3{0,0,0}, Vec3{0,i,0} }.draw(Palette::Green);
			//for (int i = 0; i < 10; ++i) Line3D{ Vec3{0,0,0}, Vec3{0,0,i} }.draw(Palette::Blue);

			// 地面の描画
			m_groundMesh.draw(m_groundTexture);

			// ゲームオブジェクトの描画
			for (const auto& obj : m_gameObject)
			{
				obj->Draw();
			}

			// プレイヤーの攻撃の当たり判定を表示（デバッグ用）
			//m_player->GetAttackSphere().draw(ColorF{ Palette::Red });
		}

		Graphics3D::Flush();
		m_renderTexture.resolve();
		Shader::LinearToScreen(m_renderTexture);
	}

	// ------------------------
	// 2D描画
	// ------------------------
	{
		// 状態遷移に応じた描画
		if (m_gameState == GameState::WaitStart)
		{
			const Vec2 pos{ Scene::Width() / 2.0, Scene::Height() * 3.0 / 10.0 };
			m_waitStartFont(U"左クリックでスタート").drawAt(pos, ColorF{ Palette::Black });
		}
		else if (m_gameState == GameState::Countdown)
		{
			// カウントダウンの描画
			if (m_countdown > 2.0)		m_countdownFont(U"3").drawAt(Scene::CenterF(), ColorF{ Palette::Black });
			else if (m_countdown > 1.0) m_countdownFont(U"2").drawAt(Scene::CenterF(), ColorF{ Palette::Black });
			else if (m_countdown > 0.0) m_countdownFont(U"1").drawAt(Scene::CenterF(), ColorF{ Palette::Black });
			else						m_countdownFont(U"スタート").drawAt(Scene::CenterF(), ColorF{ Palette::Black });
		}
		else if (m_gameState == GameState::Playing || m_gameState == GameState::Exploding)
		{
			// 制限時間の描画
			const double time = Max(0.0, m_countdown);
			const Vec2 pos{ Scene::Width() / 2.0, Scene::Height() / 10.0 };
			m_timeLimitFount(U"制限時間：{:.0f}"_fmt(time)).drawAt(pos, ColorF{ Palette::Black });

			// 敵のHPバーを描画
			if (m_enemy)
			{
				m_enemy->HPBarDraw();
			}
		}		
	}
}

