/*!
@file Scene.cpp
@brief シーン実装
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
	///	ゲームシーン
	//--------------------------------------------------------------------------------------
	void Scene::CreateResourses() {
		// アプリケーションオブジェクトを取得する
		auto& app = App::GetApp();

		// メディアフォルダーのパスを取得する
		auto mediaPath = app->GetDataDirWString();

		// リソースの読込
	}

	void Scene::OnCreate(){
		try {
			JoltManager::StaticInitialize();

			GameController::Initialize();
			GameController::EnableGyro(true);
			GameController::StartVibration(0.0f, 10.0f);

			//ステージ数1で初期化
			GameProgressManager::Get().Initialize(1);

			// 背景色を設定
			SetClearColor(Col4(0.0f, 0.11328125f, 0.2578125, 1.0f)); // ミッドナイトブルー
			
			//リソース作成
			CreateResourses();


			//自分自身にイベントを送る
			//これによりゲームステージのオブジェクトがCreate時にシーンにアクセスできる
			PostEvent(0.0f, GetThis<ObjectInterface>(), GetThis<Scene>(), L"ToTitleStage");
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
			//ゲームステージの設定
			ResetActiveStage<GameStage>();
		}
		if (event->m_MsgStr == L"ToProtoStage") {
			//ゲームステージの設定
			ResetActiveStage<ProtoStage>();
		}
		if (event->m_MsgStr == L"ToGoalStage") {
			//ゴールステージの設定
			ResetActiveStage<GoalStage>();
		}
		if (event->m_MsgStr == L"ToGameStage0") {
			ResetActiveStage<ProtoStage>();
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
