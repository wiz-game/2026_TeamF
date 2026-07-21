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
	void TitleStage::RegisterResources() {
		auto& app = App::GetApp();
		wstring mediaPath = App::GetApp()->GetDataDirWString();
		app->RegisterTexture(L"TITLE", mediaPath + L"Texture/Title1.png");
		app->RegisterTexture(L"BUTTON_A_START", mediaPath + L"Texture/Button_A_Start.png");
		app->RegisterTexture(L"PAPER", mediaPath + L"Texture/Paper.png");

	}

	TitleStage::~TitleStage()
	{
	}

	void TitleStage::OnCreate() {
		try {
			auto& app = App::GetApp();
			CreateViewLight();
			RegisterResources();

			//BGM再生
			//SoundManager::Get().PlayBGM(L"TITLE_BGM", m_BGMVolume);
			//BGM再生
			SoundManager::Get().PlayBGM(L"TITLE_BGM", m_BGMVolume);

			m_Title = AddGameObject<Sprite>(L"TITLE", Vec3(), Vec2(600, 200), Anchor::Center);
			m_Title->MatchToScreenSize();
			m_sprite_Button = AddGameObject<Sprite>(L"BUTTON_A_START", Vec3(0, -300, 0), Vec2(400, 150), Anchor::Center);

			/*shared_ptr<Sprite> paper = AddGameObject<Sprite>(L"PAPER", Vec3(), Vec2(), Anchor::Center);
			paper->MatchToScreenSize();
			paper->CreateAnimationUV(Vec2(6, 4));
			paper->AddAnimation(L"MAIN", 0, 23, 0.02f, true,true);
			paper->SetCurrentAnimation(L"MAIN");*/
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

			SoundManager::Get().PlaySE(L"CONFIRM", 1.0f);
			//BGMを止める
			SoundManager::Get().StopBGM();
			m_titleBGM = nullptr;

			m_ButtonScaleTimer = 0;//アニメーションスタート

			PostEvent(0.3f, GetThis<TitleStage>(), scene, L"ToSelectStage");
		}
		SpriteMove();
	}

	//ボタンの押し込みアニメーション
	void TitleStage::SpriteMove()
	{
		SpriteMoveUtil::CalculatePunchScale(m_ButtonScaleTimer, m_ButtonScaleRation, 0.1f);

		m_sprite_Button->SetSize(Vec2(400.0f * m_ButtonScaleRation, 150.0f * m_ButtonScaleRation));
	}

}
//end basecross
