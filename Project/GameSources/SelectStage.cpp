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
#include "Sprite.h"
namespace basecross {
	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス実体
	//--------------------------------------------------------------------------------------
	SelectStage::SelectStage() :Stage(), m_SelectIndex(0), m_MaxSelectIndex(5) {}

	SelectStage::~SelectStage()
	{
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
		app->RegisterTexture(L"BUTTON_A", mediaPath + L"Texture/Button_A_Select.png");
		app->RegisterTexture(L"BUTTON_SELECT", mediaPath + L"Texture/Button_Select.png");
		app->RegisterTexture(L"BUTTON_X", mediaPath + L"Texture/Button_A_Title_Take.png");

		for (int i = 1; i < m_MaxSelectIndex + 1; i++)
		{
			auto texName = L"STAGE" + to_wstring(i);
			auto SelectedTexName = texName + L"SELECTED";
			auto texPathName = L"Texture/Select_Stage" + to_wstring(i);
			app->RegisterTexture(texName, mediaPath + texPathName + L".png");
			app->RegisterTexture(SelectedTexName, mediaPath + texPathName + L"_SelectedTex.png");
		}
	}

	void SelectStage::OnCreate() {
		try {
			auto& app = App::GetApp();
			AddGameObject<ButtonManager>();
			m_Title = GetThis<SelectStage>();

			CreateViewLight();
			RegisterResources();

			auto bac = AddGameObject<Sprite>(L"BACKGROUND", Vec3(), Vec2(600, 200), Anchor::Center);
			bac->MatchToScreenSize();
			SpriteCreation();


			m_MaxSelectIndex = GameProgressManager::Get().GetStageSize();
			m_SelectingSprite = AddGameObject<NumberSprite>(L"NUMBER", Vec3(-25.0f, 50.0f, 0.0f), Vec2(50, 100), 2);
			m_SelectingSprite->UpdateNumber(m_SelectIndex);

			m_sprite_Buttons.push_back(AddGameObject<Sprite>(L"BUTTON_X", Vec3(-400, -300, 0), Vec2(400, 150), Anchor::Center));
			m_sprite_Buttons.push_back(AddGameObject<Sprite>(L"BUTTON_SELECT", Vec3(0, -300, 0), Vec2(400, 150), Anchor::Center));
			m_sprite_Buttons.push_back(AddGameObject<Sprite>(L"BUTTON_A", Vec3(400, -300, 0), Vec2(400, 150), Anchor::Center));

			//BGM再生
			if (!m_sSelectBGM)
				m_sSelectBGM = SoundManager::Get().PlayBGM(L"STAGESELECT", m_BGMVolume);
		}
		catch (...) {
			throw;
		}
	}

	void SelectStage::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();

		if (m_ButtonScaleTimer != -1) {
			SpriteMove();
			return;
		}

		//タイトルに戻る
		if (GameController::IsTrigger_ButtonLeft())
		{
			m_ButtonScaleTimer = 0;
			m_ButtonScaleIndex = 0;

			PostEvent(0.3f, GetThis<ObjectInterface>(), app->GetScene<Scene>(), L"ToTitleStage");
		}

		//選択移動左
		if (GameController::IsTrigger_DpadLeft() && ButtonManager::instance->GetMoveStop()) {
			m_ButtonScaleTimer = 0;
			m_ButtonScaleIndex = 1;

			if (m_SelectIndex >= 1)ButtonManager::instance->SetMoveAmount(L"SelectPage1", Vec3((1980 / 2), 0, 0), 60.0f);
			m_SelectIndex = max(0, m_SelectIndex - 1);
			m_SelectingSprite->UpdateNumber(m_SelectIndex);
			ButtonManager::instance->SetSelectIndex(L"SelectPage1", m_SelectIndex);
		}
		//選択移動右
		if (GameController::IsTrigger_DpadRight() && ButtonManager::instance->GetMoveStop()) {
			m_ButtonScaleTimer = 0;
			m_ButtonScaleIndex = 1;


			if (m_SelectIndex < m_MaxSelectIndex - 1)ButtonManager::instance->SetMoveAmount(L"SelectPage1", Vec3((-1980 / 2), 0, 0), 60.0f);
			m_SelectIndex = min(m_MaxSelectIndex - 1, m_SelectIndex + 1);
			m_SelectingSprite->UpdateNumber(m_SelectIndex);
			ButtonManager::instance->SetSelectIndex(L"SelectPage1", m_SelectIndex);
		}
		//選択決定(A)
		if (GameController::IsTrigger_ButtonDown()) {
			m_ButtonScaleTimer = 0;
			m_ButtonScaleIndex = 2;

			//BGMを止める
			SoundManager::Get().StopBGM();
			m_sSelectBGM = nullptr;

			//PostEvent(0.2f, GetThis<SelectStage>(), app->GetScene<Scene>(), L"ToProtoStage");
			PostEvent(0.0f, GetThis<ObjectInterface>(), app->GetScene<Scene>(), L"ToGameStage", make_shared<int>(m_SelectIndex));
		}
	}

	void SelectStage::SpriteMove()
	{
		SpriteMoveUtil::CalculatePunchScale(m_ButtonScaleTimer, m_ButtonScaleRation, 0.1f);

		m_sprite_Buttons[m_ButtonScaleIndex]->SetSize(Vec2(400.0f * m_ButtonScaleRation, 150.0f * m_ButtonScaleRation));
	}


	void SelectStage::SpriteCreation()
	{

		for (int i = 1; i < m_MaxSelectIndex + 1; ++i)
		{
			auto texName = L"STAGE" + to_wstring(i);
			auto texSelected = texName + L"SELECTED";

			m_Scale = Vec2(600, 600);
			ButtonManager::instance->Create(m_Title, L"SelectPage1", texName, texSelected, Vec3((((1980 / 2) * (i - 1)) - (m_Scale.x / 2)), m_Scale.y / 2, 0), m_Scale, [](shared_ptr<ObjectInterface>& object) {});
		}
	}
}
//end basecross
