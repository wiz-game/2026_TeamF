///*!
//@file GameStage.cpp
//@brief ゲームステージ実体
//*/
//
#include <algorithm>

#include "stdafx.h"
#include "Project.h"
#include "game_controller.h"
#include "GameProgressManager.h"

namespace basecross {
	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス実体
	//--------------------------------------------------------------------------------------
	SelectStage::SelectStage():Stage(),m_SelectIndex(0),m_MaxSelectIndex(3){}

	SelectStage::~SelectStage()
	{
		SoundManager::Get().StopBGM();
	}

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

	void SelectStage::RegisterResources() {
		auto& app = App::GetApp();
		wstring mediaPath = App::GetApp()->GetDataDirWString();
		app->RegisterTexture(L"BACKGROUND", mediaPath + L"Texture/background.jpg");
		app->RegisterTexture(L"BLACK", mediaPath + L"Texture/Black.png");
		app->RegisterTexture(L"NUMBER", mediaPath + L"Texture/Number.png");
	}

	void SelectStage::OnCreate() {
		try {
			auto& app = App::GetApp();
			CreateViewLight();
			RegisterResources();

			m_Title = AddGameObject<Sprite>(L"BACKGROUND", Vec3(), Vec2(600, 200), Anchor::Center);
			m_Title->MatchToScreenSize();
			SpriteCreation();

			m_MaxSelectIndex = GameProgressManager::Get().GetStageSize();
			m_SelectingSprite = AddGameObject<NumberSprite>(L"NUMBER", Vec3(-25.0f, 50.0f, 0.0f), Vec2(50, 100), 2);
			m_SelectingSprite->UpdateNumber(m_SelectIndex);

			//BGM再生
			SoundManager::Get().PlayBGM(L"TITLE_BGM", m_BGMVolume);



		}
		catch (...) {
			throw;
		}
	}

	void SelectStage::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();

		//ステージ0選択用
		if (GameController::IsTrigger_ButtonUp()) {
			m_SelectIndex = -1;
			m_SelectingSprite->UpdateNumber(m_SelectIndex);
		}

		//選択移動左
		if (GameController::IsTrigger_DpadLeft()) {
			m_SelectIndex = max(0, m_SelectIndex - 1);
			m_SelectingSprite->UpdateNumber(m_SelectIndex);
		}
		//選択移動右
		if (GameController::IsTrigger_DpadRight()) {
			m_SelectIndex = min(m_MaxSelectIndex - 1, m_SelectIndex + 1);
			m_SelectingSprite->UpdateNumber(m_SelectIndex);
		}
		//選択決定(A)
		if (GameController::IsTrigger_ButtonDown()) {
			//PostEvent(0.2f, GetThis<SelectStage>(), app->GetScene<Scene>(), L"ToGameStage" + to_wstring(m_SelectIndex));
			PostEvent(0.0f, GetThis<ObjectInterface>(), app->GetScene<Scene>(), L"ToGameStage"+ to_wstring(m_SelectIndex));
		}

		TitleControl();
	}

	void SelectStage::SpriteCreation()
	{
		m_StageSprites.clear();
		for (int i = 0; i < m_MaxSelectIndex; ++i) {
			auto texName = L"BLACK"; 
			auto sprite = AddGameObject<Sprite>(texName, Vec3(), Vec2(180, 120), Anchor::Center);
			m_StageSprites.push_back(sprite);
		}
	}

	void SelectStage::TitleControl()
	{

	}
}
//end basecross
