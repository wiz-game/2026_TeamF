/*!
@file GameStage.cpp
@brief ゲームステージ実体
*/

#include "stdafx.h"
#include "Project.h"
#include "game_controller.h"
namespace basecross {
	IMPLEMENT_DX11_COMPUTE_SHADER(TestShader,App::GetApp()->GetShadersPath() + L"TestComputeShader.cso")
	IMPLEMENT_DX11_CONSTANT_BUFFER(TestConstantBuffer)
	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス実体
	//--------------------------------------------------------------------------------------

	//ビューとライトの作成
	void GameStage::CreateViewLight() {
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

	void GameStage::OnCreate() {
		try {
			auto& app = App::GetApp();

			// JoltPhysicsを初期化する
			m_jphManger.Initialize();

			//ビューとライトの作成
			CreateViewLight();
			App::GetApp()->RegisterTexture(L"InkTest",App::GetApp()->GetDataDirWString() +  L"Texture/InkCollisionTest.png");

			m_Player = AddGameObject<Player>();
			AddGameObject<InkDraw>();
			AddGameObject<PowerSupply>();
			AddGameObject<Port>();

			auto draw = m_Player->GetComponent<SmBaseDraw>();
			draw->SetTextureResource(L"InkTest");
			m_Player->AddComponent<TextureCollision>();

			vector<float> testData(5000000, 1.0f);
			vector<float> result = {};

			auto start = std::chrono::steady_clock::now();
			DX11ComputeShader<float> shader = DX11ComputeShader<float>();
			shader.Initialize(256, testData.size(), testData.size());
			shader.SetShader(TestShader::GetPtr()->GetShader());
			
			result = shader.Execute(testData);
			auto end = std::chrono::steady_clock::now();

			auto duration = std::chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f;
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
		m_jphManger.Update(1.0f / 60.0f);
	}

	void GameStage::OnDraw()
	{
	}
}
//end basecross
