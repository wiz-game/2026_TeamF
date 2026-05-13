/*!
@file GameStage.cpp
@brief ゲームステージ実体
*/

#include "stdafx.h"
#include "Project.h"
#include "game_controller.h"
namespace basecross {
	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス実体
	//--------------------------------------------------------------------------------------

	//ビューとライトの作成
	void GoalStage::CreateViewLight() {
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

	void GoalStage::OnCreate() {
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

	void GoalStage::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();
		auto scene = app->GetScene<Scene>();
		//GameController::Update();
		
		m_InputHandler.PushHandle(GetThis<GoalStage>());

		auto CntlVec = app->GetInputDevice().GetControlerVec();

		scene->SetDebugString(L"GoalStage \n Abutton -> TitleStage");

		if (CntlVec[0].wPressedButtons && XINPUT_GAMEPAD_A)
		{
			PostEvent(0.3f, GetThis<GoalStage>(), scene, L"ToTitleStage");
		}
	}

	void GoalStage::OnUpdate2()
	{
	}

	void GoalStage::OnDraw()
	{
	}

	void GoalStage::OnPushA()
	{
	}
}
//end basecross
