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
	void GoalStage::RegisterResources() {
		auto& app = App::GetApp();
		wstring mediaPath = App::GetApp()->GetDataDirWString();
		app->RegisterTexture(L"GoalStage", mediaPath + L"Texture/GoalStage.png");
		app->RegisterTexture(L"BUTTON_A", mediaPath + L"Texture/Button_A.png");
		app->RegisterTexture(L"NUMBER", mediaPath + L"Texture/Number.png");

	}

	void GoalStage::OnCreate() {
		try {
			auto& app = App::GetApp();
			auto scene = app->GetScene<Scene>();
			CreateViewLight();
			RegisterResources();

			m_sprite = AddGameObject<Sprite>(L"GoalStage", Vec3(), Vec2(1280, 840), Anchor::Center);
			m_sprite_Button = AddGameObject<Sprite>(L"BUTTON_A", Vec3(0, -300, 0), Vec2(400, 150), Anchor::Center);

			float ink = scene->GetResultInk();
			float maxInk = scene->GetMaxInk();

			int pasent = (ink / maxInk) * 100;

			auto inkSprite = AddGameObject<NumberSprite>(L"NUMBER", Vec3(140.0f, -65.0f, 0.0f), Vec2(30, 50), 2);
			inkSprite->SetDiffuse(Col4(0, 0, 0, 1));
			inkSprite->UpdateNumber(pasent);

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

		//scene->SetDebugString(L"GoalStage \n Abutton -> TitleStage");

		if (CntlVec[0].wPressedButtons && XINPUT_GAMEPAD_A)
		{
			PostEvent(0.3f, GetThis<GoalStage>(), scene, L"ToTitleStage");
		}
	}
	void GoalStage::OnPushA()
	{
	}
}
//end basecross
