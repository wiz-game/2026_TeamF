/*!
@file Scene.cpp
@brief 繧ｷ繝ｼ繝ｳ螳溯｣・
*/

#include "stdafx.h"
#include "Project.h"
#include "DXModelResource.h"
#include "JoltManager.h"
#include "Scene.h"
#include "game_controller.h"
#include "GameProgressManager.h"

namespace basecross{

	//--------------------------------------------------------------------------------------
	///	
	//--------------------------------------------------------------------------------------
	void Scene::CreateResourses() {
		auto& app = App::GetApp();

		auto mediaPath = app->GetDataDirWString();
		app->RegisterTexture(L"SKYBOX", mediaPath + L"Texture/SkyBox/SkyBox.png");
	}

	void Scene::OnCreate(){
		try {
			JoltManager::StaticInitialize();

			GameController::Initialize();
			GameController::EnableGyro(true);
			GameController::StartVibration(0.0f, 10.0f);

			//ステージ数1で初期化
			GameProgressManager::Get().Initialize(3);

			SetClearColor(Col4(0.0f, 0.11328125f, 0.2578125, 1.0f));
			
			CreateResourses();


			//自分自身にイベントを送る
			//これによりゲームステージのオブジェクトがCreate時にシーンにアクセスできる
			PostEvent(0.0f, GetThis<ObjectInterface>(), GetThis<Scene>(), L"ToProtoStage");
		}
		catch (...) {
			throw;
		}
	}
	void Scene::OnUpdate() {
		SceneBase::OnUpdate();
		GameController::Update();
	}
	Scene::~Scene() {
		JoltManager::StaticTerminate();
	}

	void Scene::OnEvent(const shared_ptr<Event>& event) {
		if (event->m_MsgStr == L"ToGameStage") {
			ResetActiveStage<GameStage>();
		}
		if (event->m_MsgStr == L"ToProtoStage") {
			ResetActiveStage<ProtoStage>();
		}
		if (event->m_MsgStr == L"ToGoalStage") {
			ResetActiveStage<GoalStage>();
		}

		//仮で設定。後々やり方変える
		if (event->m_MsgStr == L"ToGameStage0") {
			ResetActiveStage<ProtoStage>();
		}
		//if (event->m_MsgStr == L"ToGameStage1") {
		//	ResetActiveStage<GameStage>();
		//}
		//if (event->m_MsgStr == L"ToGameStage2") {
		//	ResetActiveStage<GoalStage>();
		//}
		if (event->m_MsgStr == L"ToGameStage0") {
			//ResetActiveStage<ProtoStage>();
			ResetActiveStage<GameStageBase>();
		}


		if (event->m_MsgStr == L"ToTitleStage") {
			ResetActiveStage<TitleStage>();
		}
		if (event->m_MsgStr == L"ToSelectStage") {
			ResetActiveStage<SelectStage>();
		}

	}

	void Scene::OnDestroy() {
		GameController::Finalize();
	}
}
//end basecross
