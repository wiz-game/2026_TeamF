/*!
@file GameOverStage.cpp
@brief ゲームオーバーステージ実体
*/

#include "stdafx.h"
#include "Project.h"
#include "game_controller.h"
namespace basecross {
	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス実体
	//--------------------------------------------------------------------------------------

	//ビューとライトの作成
	void GameOverStage::CreateViewLight() {
		// カメラの設定
		auto camera = ObjectFactory::Create<Camera>();
		camera->SetEye(Vec3(0.0f, 8.0f, -8.0f));
		camera->SetAt(Vec3(0.0f, 0.0f, 0.0f));

		// ビューにカメラを設定
		auto view = CreateView<SingleView>();
		view->SetCamera(camera);

		//マルチライトの作成
		auto light = CreateLight<MultiLight>();
		light->SetDefaultLighting(); //デフォルトのライティングを指定
	}
	void GameOverStage::RegisterResources() {
		auto& app = App::GetApp();
		wstring mediaPath = App::GetApp()->GetDataDirWString();
		app->RegisterTexture(L"GameOver", mediaPath + L"Texture/GameOverStage.png");
		app->RegisterTexture(L"BUTTON_A_TITLE", mediaPath + L"Texture/Button_A_Title.png");

	}

	void GameOverStage::OnCreate() {
		try {
			auto& app = App::GetApp();

			RegisterResources();
			// JoltPhysicsを初期化する
			//m_jphManger.Initialize();

			//ビューとライトの作成
			CreateViewLight();

			m_sprite = AddGameObject<Sprite>(L"GameOver", Vec3(), Vec2(1280, 840), Anchor::Center);
			m_sprite_Button = AddGameObject<Sprite>(L"BUTTON_A_TITLE", Vec3(0,-255,0), Vec2(400, 150), Anchor::Center);

			SoundManager::Get().PlayBGM(L"GAMEOVER_BGM", m_BGMVolume);
		}
		catch (...) {
			throw;
		}
	}

	void GameOverStage::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();
		auto scene = app->GetScene<Scene>();
		//GameController::Update();

		m_InputHandler.PushHandle(GetThis<GameOverStage>());

		auto CntlVec = app->GetInputDevice().GetControlerVec();


		if (CntlVec[0].wPressedButtons && XINPUT_GAMEPAD_A)
		{
			SoundManager::Get().StopBGM();

			m_ButtonScaleTimer = 0;
			PostEvent(0.3f, GetThis<GameOverStage>(), scene, L"ToTitleStage");
		}
		SpriteMove();
	}
	void GameOverStage::SpriteMove()
	{
		SpriteMoveUtil::CalculatePunchScale(m_ButtonScaleTimer, m_ButtonScaleRation, 0.1f);

		m_sprite_Button->SetSize(Vec2(400.0f * m_ButtonScaleRation, 150.0f * m_ButtonScaleRation));
	}

	void GameOverStage::OnUpdate2()
	{
	}

	void GameOverStage::OnDraw()
	{
	}

	void GameOverStage::OnPushA()
	{
	}

}
//end basecross
