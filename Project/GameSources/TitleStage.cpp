///*!
//@file GameStage.cpp
//@brief ゲームステージ実体
//*/
//
#include "stdafx.h"
#include "Project.h"
#include "game_controller.h"
namespace basecross {
	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス実体
	//--------------------------------------------------------------------------------------

	//ビューとライトの作成
	void TitleStage::CreateViewLight() {
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

	void TitleStage::OnCreate() {
		try {
			auto& app = App::GetApp();
			CreateViewLight();
		}
		catch (...) {
			throw;
		}
	}

	void TitleStage::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();
		if (!GameController::IsConnected()) return;

		//Aボタン
		if (GameController::IsTrigger_ButtonDown()) {
			auto& scene = app->GetScene<Scene>();
			PostEvent(0.0f, GetThis<TitleStage>(), scene, L"ToSelectStage");
		}
	}
}
//end basecross
