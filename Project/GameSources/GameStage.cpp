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
	void GameStage::CreateViewLight() {
		// カメラの設定
		auto camera = ObjectFactory::Create<MainCamera>();
		camera->SetEye(Vec3(0.0f, 8.0f, -8.0f));
		camera->SetAt(Vec3(0.0f, 0.0f, 0.0f));

		// ビューにカメラを設定
		auto view = CreateView<SingleView>();
		view->SetCamera(camera);

		//マルチライトの作成
		auto light = CreateLight<MultiLight>();
		light->SetDefaultLighting(); //デフォルトのライティングを指定

		
	}

	void GameStage::OnCreate() {
		try {
			auto& app = App::GetApp();

			// JoltPhysicsを初期化する
			m_jphManger.Initialize();

			//ビューとライトの作成
			CreateViewLight();
			AddGameObject<EffectManager>();

			wstring mediaPath = App::GetApp()->GetDataDirWString();
			EffectManager::g_Instance->RegisterResource(L"ELECTRIC", mediaPath + L"Effects/Electric2.efk");

			App::GetApp()->RegisterTexture(L"InkTest",App::GetApp()->GetDataDirWString() +  L"Texture/Test/InkCollisionTest2.png");
			//App::GetApp()->RegisterTexture(L"InkNormalMap", App::GetApp()->GetDataDirWString() + L"Texture/Test/InkNormalMap.jpg");

			m_Player = AddGameObject<Player>(Vec3(0,1,0),Vec3(), Vec3(),float(10.0f));
			AddGameObject<InkDraw>();
			AddGameObject<PowerSupply>(Vec3(1.0f, 0.1f, 1.0f), Vec3(0), Vec3(0.0f, -0.5f, -4.0f));
			AddGameObject<Port>(Vec3(1.0f, 0.1f, 1.0f), Vec3(0), Vec3(0.0f, -0.5f, 3.0f));

			auto view = GetView();
			auto camera = view->GetTargetCamera();
			auto mainCamera = dynamic_pointer_cast<MainCamera>(camera);
			mainCamera->SetTarget(m_Player);

			auto floor = AddGameObject<Floor>(Vec3(10,1.0f,10.0f), Vec3(0), Vec3(0, -1.0f, 0));
			//auto draw = floor->GetComponent<InkDrawComponentTest>();
			//draw->SetNormalMap(App::GetApp()->GetDataDirWString() + L"Texture/Test/InkNormalMap.dds");
			//draw->SetBrushSize(0.5f);
			//draw->SetTextureResource(L"SKYBOX");
			floor->AddComponent<TextureCollision>();
			m_TestFloors.push_back(floor);
			//float sizeX = 4.0f, sizeZ = 4.0f;
			//for (int i = 0; i < 1; i++) {
			//	for (int j = 0; j < 2; j++) {
			//		float x = -sizeX + j * sizeX * 2.0f;
			//		float y = -1.0f;
			//		float z = -sizeZ + i * sizeZ * 2.0f;

			//		auto floor = AddGameObject<Floor>(Vec3(sizeX * 2, 1, sizeX * 4),Vec3(0), Vec3(x, y, z));
			//		auto draw = floor->GetComponent<InkDrawComponentTest>();
			//		//draw->SetNormalMap(App::GetApp()->GetDataDirWString() + L"Texture/Test/InkNormalMap.dds");
			//		draw->SetBrushSize(0.5f);
			//		//draw->SetTextureResource(L"SKYBOX");
			//		floor->AddComponent<TextureCollision>();
			//		m_TestFloors.push_back(floor);
			//	}
			//}

			
		}
		catch (...) {
			throw;
		}
	}

	void GameStage::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();
		GameController::Update();
	}

	void GameStage::OnUpdate2()
	{
		//m_jphManger.Update(1.0f / 60.0f);
		//TextureMeshManager::Get().Reload();
	}

	void GameStage::OnDraw()
	{
	}
}
//end basecross
