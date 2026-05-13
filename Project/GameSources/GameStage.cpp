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

		vector<VertexPositionColor> vertices = {
			{{0.0f,0.0f,0.0f},{1,1,1}},
			{{0.0f,0.0f,1.0f},{1,1,1}}
		};
		vector<uint16_t> indices{
			0,1
		};

		App::GetApp()->RegisterResource(L"DEFAULT_PC_LINE", MeshResource::CreateMeshResource(vertices, indices, false));
	}

	void GameStage::OnCreate() {
		try {
			auto& app = App::GetApp();

			// JoltPhysicsを初期化する
			m_jphManger.Initialize();

			//ビューとライトの作成
			CreateViewLight();
			App::GetApp()->RegisterTexture(L"InkTest",App::GetApp()->GetDataDirWString() +  L"Texture/Test/InkCollisionTest3.png");

			m_Player = AddGameObject<Player>(Vec3(0,1,0),Vec3(), Vec3());
			AddGameObject<InkDraw>();

			//AddGameObject<PowerSupply>(Vec3(0.0f, -0.3f, -4.0f));
			//AddGameObject<Port>(Vec3(0.0f, -0.3f, 3.0f));

			auto view = GetView();
			auto camera = view->GetTargetCamera();
			auto mainCamera = dynamic_pointer_cast<MainCamera>(camera);
			mainCamera->SetTarget(m_Player);

			float sizeX = 4.0f, sizeZ = 7.5f;
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 2; j++) {
					float x = -sizeX + j * sizeX * 2.0f;
					float y = -1.0f;
					float z = -sizeZ + i * sizeZ * 2.0f;

					auto floor = AddGameObject<Floor>(Vec3(8, 1, 15), Vec3(0), Vec3(x, y, z));
					auto draw = floor->GetComponent<SmBaseDraw>();
					draw->SetTextureResource(L"InkTest");
					floor->AddComponent<TextureCollision>();
					m_TestFloors.push_back(floor);
				}
			}

			
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
		for (auto& floor : m_TestFloors) {
			TextureMeshManager::Get().AddReload(floor->GetComponent<TextureCollision>());
		}
	}

	void GameStage::OnUpdate2()
	{
		m_jphManger.Update(1.0f / 60.0f);
		TextureMeshManager::Get().Reload();
	}

	void GameStage::OnDraw()
	{
	}
}
//end basecross
