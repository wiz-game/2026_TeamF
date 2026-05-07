///*!
//@file GameStage.cpp
//@brief ゲームステージ実体
//*/
//
#include "stdafx.h"
#include "Project.h"
#include "game_controller.h"
#include "GameProgressManager.h"

namespace basecross {
	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス実体
	//--------------------------------------------------------------------------------------
	SelectStage::SelectStage():Stage(),m_SelectIndex(0),m_MaxSelectIndex(1){}

	//ビューとライトの作成
	void SelectStage::CreateViewLight() {
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

	void SelectStage::OnCreate() {
		try {
			auto& app = App::GetApp();
			CreateViewLight();

			m_MaxSelectIndex = GameProgressManager::Get().GetStageSize();
		}
		catch (...) {
			throw;
		}
	}

	void SelectStage::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();

		//選択移動左
		if (GameController::IsPressed_DpadLeft()) {
			m_SelectIndex = max(0, m_SelectIndex - 1);
		}
		//選択移動右
		if (GameController::IsPressed_DpadRight()) {
			m_SelectIndex = min(m_MaxSelectIndex - 1, m_SelectIndex + 1);
		}
		//選択決定(A)
		if (GameController::IsTrigger_ButtonDown()) {
			PostEvent(0.0f, GetThis<SelectStage>(), app->GetScene<Scene>(), L"ToGameStage" + to_wstring(m_SelectIndex));
		}
	}
}
//end basecross
