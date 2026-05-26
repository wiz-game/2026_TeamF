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

	void GameOverStage::OnCreate() {
		try {
			auto& app = App::GetApp();

			// JoltPhysicsを初期化する
			//m_jphManger.Initialize();

			//ビューとライトの作成
			CreateViewLight();

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

		scene->SetDebugString(L"GameOverStage		Player is Ink == 0 !!! \n Abutton -> TitleStage");

		if (CntlVec[0].wPressedButtons && XINPUT_GAMEPAD_A)
		{
			PostEvent(0.3f, GetThis<GameOverStage>(), scene, L"ToTitleStage");
		}
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
