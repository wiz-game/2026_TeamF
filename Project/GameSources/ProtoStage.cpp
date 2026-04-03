/*!
@file GameStage.cpp
@brief プロトタイプステージ実体
*/

#include "stdafx.h"
#include "Project.h"
#include "game_controller.h"
#include "JoltRigidBody.h"
#include "CharacterController.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	プロトタイプステージクラス実体
	//--------------------------------------------------------------------------------------

	//ビューとライトの作成
	void ProtoStage::CreateViewLight() {
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

	void ProtoStage::OnCreate() {
		try {
			auto& app = App::GetApp();

			// JoltPhysicsを初期化する
			m_jphManger.Initialize();

			//ビューとライトの作成
			CreateViewLight();

			//プレイヤー作成
			m_Player = AddGameObject<Player>();
			//SetSharedGameObject(L"Player", m_Player);

			//カメラ取得
			auto view = GetView();
			auto camera = view->GetTargetCamera();
			auto mainCamera = dynamic_pointer_cast<MainCamera>(camera);
			mainCamera->SetTarget(m_Player);

			//プロトタイプ用地面作成
			//JPH::StaticCompoundShapeSettings compoundSettings;
			//JPH::BoxShapeSettings floorShapeSettings(JPH::Vec3(8.0f, 1.0f, 15.0f) * 0.45f);
			//JPH::ShapeRefC floorShape = floorShapeSettings.Create().Get();
			//compoundSettings.AddShape(JPH::Vec3(0.0f, -1.0f, 0.0f), JPH::Quat::sIdentity(), floorShape);

			//auto level = AddGameObject<GameObject>();
			//auto rb = level->AddComponent<JoltRigidBody>();

			////JoltRigidBody
			//JoltRigidBody::Settings settings;
			//settings.shape = floorShape;
			//settings.motionType = JPH::EMotionType::Static;
			//settings.objectLayer = Layers::NON_MOVING;

			//rb->Initialize(settings);

			//地面作成
			m_floor = AddGameObject<Floor>();

			AddGameObject<PowerSupply>();
			AddGameObject<Port>();
			//AddGameObject<Goal>();
			AddGameObject<UpDownFloor>();


		}
		catch (...) {
			throw;
		}
	}

	void ProtoStage::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();
		GameController::Update();
	}

	void ProtoStage::OnUpdate2()
	{

	}

	void ProtoStage::OnDraw()
	{
	}
}
//end basecross
