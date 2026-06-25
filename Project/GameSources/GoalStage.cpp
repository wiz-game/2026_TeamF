/*!
@file GameStage.cpp
@brief ゲームステージ実体
*/

#include "stdafx.h"
#include "Project.h"
#include "game_controller.h"
#include "GameProgressManager.h"
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
		app->RegisterTexture(L"ClearMark", mediaPath + L"Texture/ClearMark.png");
		app->RegisterTexture(L"Map", mediaPath + L"Texture/ResultMapProto.png");
		app->RegisterTexture(L"BUTTON_A_TITLE", mediaPath + L"Texture/Button_A_Title_Take.png");
		app->RegisterTexture(L"BUTTON_B_SELECT", mediaPath + L"Texture/Button_B_Select.png");
		app->RegisterTexture(L"BUTTON_X_NEXT", mediaPath + L"Texture/Button_X_Next.png");
		app->RegisterTexture(L"NUMBER", mediaPath + L"Texture/Number.png");

	}

	void GoalStage::OnCreate() {
		try {
			auto& app = App::GetApp();
			auto scene = app->GetScene<Scene>();
			CreateViewLight();
			RegisterResources();

			m_sprite = AddGameObject<Sprite>(L"GoalStage", Vec3(), Vec2(1280, 840), Anchor::Center);
			AddGameObject<Sprite>(L"Map", Vec3(0.0f,80.0f,0.0f), Vec2(350.0f, 210.0f), Anchor::Center);
			m_sprite_Buttons.push_back(AddGameObject<Sprite>(L"BUTTON_A_TITLE", Vec3(-400, -300, 0), Vec2(400, 150), Anchor::Center));
			m_sprite_Buttons.push_back(AddGameObject<Sprite>(L"BUTTON_B_SELECT", Vec3(0, -300, 0), Vec2(400, 150), Anchor::Center));
			if (GameProgressManager::Get().IsExistsNextStage()) {
				m_sprite_Buttons.push_back(AddGameObject<Sprite>(L"BUTTON_X_NEXT", Vec3(400, -300, 0), Vec2(400, 150), Anchor::Center));
			}

			float ink = scene->GetResultInk();
			float maxInk = scene->GetMaxInk();

			int pasent = (ink / maxInk) * 100;

			auto inkSprite = AddGameObject<NumberSprite>(L"NUMBER", Vec3(65.0f, -60.0f, 0.0f), Vec2(30, 50), 3);
			inkSprite->SetDiffuse(Col4(0, 0, 0, 1));
			inkSprite->UpdateNumber(pasent);

			m_ClearStanp = AddGameObject<StanpSprite>(L"ClearMark", Vec3(0.0f, 80.0f, 0.0f), Vec2(350.0f, 210.0f), 0.35f, 1.5f, 0.8f);
			m_ClearStanp->SetSE(L"STAGESELECT");
			m_ClearStanp->StartAnimation();

			GameProgressManager::Get().ClearCurrentStage();
			
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
		if (m_ButtonScaleTimer != -1) {
			SpriteMove();
			return;
		}

		if (CntlVec[0].wPressedButtons & XINPUT_GAMEPAD_X)
		{
			m_ButtonScaleTimer = 0;
			m_ButtonScaleIndex = 0;
			PostEvent(0.3f, GetThis<GoalStage>(), scene, L"ToTitleStage");
		}
		if (CntlVec[0].wPressedButtons & XINPUT_GAMEPAD_A)
		{
			m_ButtonScaleTimer = 0;
			m_ButtonScaleIndex = 1;
			PostEvent(0.3f, GetThis<GoalStage>(), scene, L"ToSelectStage");
		}

		if (!GameProgressManager::Get().IsExistsNextStage()) return;
		if (CntlVec[0].wPressedButtons & XINPUT_GAMEPAD_B)
		{
			m_ButtonScaleTimer = 0;
			m_ButtonScaleIndex = 2;
			int currentStage = GameProgressManager::Get().GetCurrentStage();
			currentStage++;
			PostEvent(0.3f, GetThis<GoalStage>(), scene, L"ToGameStage", make_shared<int>(currentStage));
		}
		
		
	}

	//ボタンの押し込みアニメーション
	void GoalStage::SpriteMove()
	{
		SpriteMoveUtil::CalculatePunchScale(m_ButtonScaleTimer, m_ButtonScaleRation, 0.1f);

		m_sprite_Buttons[m_ButtonScaleIndex]->SetSize(Vec2(400.0f * m_ButtonScaleRation, 150.0f * m_ButtonScaleRation));
	}


	void GoalStage::OnPushA()
	{
	}




	StanpSprite::StanpSprite(
		const shared_ptr<Stage>& ptr,
		const wstring& tex, const Vec3& position, const Vec2& scale,
		float animationTime, float maxScaling, float minScaling) :
		GameObject(ptr),m_TexKey(tex), m_Postion(position),m_DefaultScale(scale),
		m_IsPlyedSE(false), m_SEKey(L""),
		m_IsAnimationUpdate(false),m_AnimaitionFactor(0.0f),m_AnimationTime(animationTime),m_MaxScaling(maxScaling),m_MinScaling(minScaling)
	{

	}


	void StanpSprite::OnCreate() {
		m_Sprite = GetStage()->AddGameObject<Sprite>(m_TexKey, m_Postion, Vec2(0.0f), Anchor::Center);
	}
	void StanpSprite::OnUpdate() {
		if (!m_IsAnimationUpdate) return;

		float delta = App::GetApp()->GetElapsedTime();
		m_AnimaitionFactor += ( 1.0f / m_AnimationTime ) * delta;
		m_AnimaitionFactor = clamp(m_AnimaitionFactor, 0.0f, 1.0f);

		float downScalingFactor = 0.9f;
		float upScalingFactor = 1.0f - downScalingFactor;
		float currentScaling = 1.0f;
		if (m_AnimaitionFactor <= downScalingFactor) {
			float factor = m_AnimaitionFactor / downScalingFactor;
			currentScaling = m_MaxScaling - ( m_MaxScaling - m_MinScaling ) * factor;
		}
		else {
			float factor = (m_AnimaitionFactor - downScalingFactor) / upScalingFactor;
			currentScaling = m_MinScaling + (1.0f - m_MinScaling) * factor;
		}

		m_Sprite->SetSize(m_DefaultScale * currentScaling);

		if (!m_IsPlyedSE && m_SEKey != L"" && m_AnimaitionFactor > downScalingFactor) {
			SoundManager::Get().PlaySE(m_SEKey, 1.0f);
			m_IsPlyedSE = true;
		}
	}

	void StanpSprite::StartAnimation() {
		m_AnimaitionFactor = 0.0f;
		m_IsAnimationUpdate = true;
		m_IsPlyedSE = false;
	}
}
//end basecross
