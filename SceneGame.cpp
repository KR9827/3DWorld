#include "SceneGame.h"
#include "GameObject.h"
#include "Player.h"
#include "Enemy.h"
#include "Camera.h"

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

	if (KeyEnter.down())
	{
		changeScene(SceneState::RESULT);
	}

	const double deltaTime = Scene::DeltaTime();
	for (auto& obj : m_gameObject)
	{
		obj->Update(static_cast<float>(deltaTime));
	}

	Print << U"プレイヤーの位置：{}"_fmt(m_player->GetPosition());
	Print << U"プレイヤーの角度：{}"_fmt(m_player->GetRotation());
	Print << U"カメラの位置：{}"_fmt(m_camera->GetPosition());
	Print << U"カメラの角度：{}"_fmt(m_camera->GetRotation());
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
			for (int i = 0; i < 10; ++i) Line3D{ Vec3{0,0,0}, Vec3{i,0,0} }.draw(Palette::Red);
			for (int i = 0; i < 10; ++i) Line3D{ Vec3{0,0,0}, Vec3{0,i,0} }.draw(Palette::Green);
			for (int i = 0; i < 10; ++i) Line3D{ Vec3{0,0,0}, Vec3{0,0,i} }.draw(Palette::Blue);



			for (const auto& obj : m_gameObject)
			{
				obj->Draw();
			}

		}

		Graphics3D::Flush();
		m_renderTexture.resolve();
		Shader::LinearToScreen(m_renderTexture);
	}

	// ------------------------
	// 2D描画
	// ------------------------
	{
		
	}
}

