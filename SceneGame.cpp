#include "SceneGame.h"
#include "GameObject.h"
#include "Player.h"
#include "Camera.h"
#include "FBXModelComponent.h"
#include "AnimationComponent.h"

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
	// プレイヤーにコンポーネントを追加
	auto model = m_player->AddComponent<FBXModelComponent>(U"Assets/fbx/Walking.fbx", 100);
	m_player->AddComponent<AnimationComponent>(model->GetSkeleton(), 120);					// スケルトンのポインタをAnimationComponentに渡す


	// カメラ生成
	m_camera = AddGameObject<Camera>();
	m_camera->SetTarget(m_player);			// 追従対象を指定
	m_camera->SetPosition(Vec3{ 0, 5, -10 });

	m_sphere = Sphere{ Vec3{0, 0, 0}, 2.0 };

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
		obj->Update(deltaTime);
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



			// 3D描画の基本ステート（お約束）設定
			//const ScopedRenderStates3D blend{ BlendState::Opaque };					// 不透明描画設定：後ろのものを透かさない
			//const ScopedRenderStates3D depth{ DepthStencilState::Default3D };		// 深度（ｚバッファ）設定：手前を優先して後ろのものを描かない
			//const ScopedRenderStates3D raster{ RasterizerState::Default3D };		// ラスタライズ設定：ポリゴンの裏表を判別し、裏側を描画しないようにする（カリング）

			//const ScopedRenderStates3D renderStates{ BlendState::Opaque, DepthStencilState::Default3D, RasterizerState::Default3D };


			// 座標軸を表示する（赤：X, 緑：Y, 青：Z）
			for (int i = 0; i < 10; ++i) Line3D{ Vec3{0,0,0}, Vec3{i,0,0} }.draw(Palette::Red);
			for (int i = 0; i < 10; ++i) Line3D{ Vec3{0,0,0}, Vec3{0,i,0} }.draw(Palette::Green);
			for (int i = 0; i < 10; ++i) Line3D{ Vec3{0,0,0}, Vec3{0,0,i} }.draw(Palette::Blue);



			for (const auto& obj : m_gameObject)
			{
				obj->Draw();
			}

			Sphere{ Vec3{0, 0, 10.0}, 3 }.draw(ColorF{ Palette::Red });


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

