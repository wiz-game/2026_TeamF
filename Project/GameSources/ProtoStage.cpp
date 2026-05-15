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
			SetSharedGameObject(L"player", m_Player);

			//カメラ取得
			auto view = GetView();
			auto camera = view->GetTargetCamera();
			auto mainCamera = dynamic_pointer_cast<MainCamera>(camera);
			mainCamera->SetTarget(m_Player);

			//UI作成
			//auto gaugeBack = AddGameObject<GaugeBack>();
			auto gauge = AddGameObject<InkGauge>();

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

			//地面						position				scale
			AddGameObject<Floor>(Vec3(0.0f, -1.0f, 0.0f), Vec3(8, 1, 15));
			AddGameObject<Floor>(Vec3(10.0f, 2.0f, 15.0f), Vec3(8.0f, 0.5f, 10.0f));
			AddGameObject<Floor>(Vec3(10.0f, 2.0f, 29.0f), Vec3(8.0f, 0.5f, 10.0f));


			AddGameObject<PowerSupply>(Vec3(0.0f, -0.3f, -4.0f));
			AddGameObject<PowerSupply>(Vec3(10.0f, 2.4f, 11.0f));

			//port										position
			auto moveFloor_port = AddGameObject<Port>(Vec3(0.0f, -0.3f, 3.0f));
			auto trapDoor_port = AddGameObject<Port>(Vec3(10.0f, 2.4f, 15.0f));
			auto goal_port = AddGameObject<Port>(Vec3(10.0f, 2.4f, 25.0f));

			//Goal					position			portの指定(nullptrの場合最初から表示)
			AddGameObject<Goal>(Vec3(10.0f, 4.0f, 33.0f), goal_port);
			
			//左に開く扉
			auto leftDoor = AddGameObject<GoalDoor>(Vec3(8.0f, 4.0f, 30.0f), goal_port, Vec3(-1, 0, 0));
			//右に開く扉
			auto rightDoor = AddGameObject<GoalDoor>(Vec3(12.0f, 4.0f, 30.0f), goal_port, Vec3(1, 0, 0));

			//トラップドアの初期設定
			TrapDoorAxisDesc moveDoorY;
			moveDoorY.pos = Vec3(10.0f, 2.2f, 20.0f);
			moveDoorY.scale = Vec3(2.0f, 0.1f, 4.0f);
			moveDoorY.initialRotation = Vec3(XM_PIDIV2, 0.0f, 0.0f);
			moveDoorY.axis = MoveAxis::X;
			moveDoorY.speed = -0.01f;
			moveDoorY.port = trapDoor_port;
			AddGameObject<TrapDoorAxis>(moveDoorY);


			//動く床の初期設定
			MoveFloorDesc moveFloorY;
			moveFloorY.pos = Vec3(0.0f, 1.5f, 5.5f);
			moveFloorY.scale = Vec3(2.0f, 0.1f, 2.0f);
			moveFloorY.axis = MoveAxis::Y;		//移動する軸の指定
			moveFloorY.speed = 1.0f;			//移動速度
			moveFloorY.limitDist = 2.05f;		//移動上限
			moveFloorY.port = moveFloor_port;	//portの指定
			AddGameObject<MoveFloor>(moveFloorY);


			MoveFloorDesc moveFloorX;
			moveFloorX.pos = Vec3(3.5f, 2.5f, 5.5f);
			moveFloorX.scale = Vec3(2.0f, 0.1f, 2.0f);
			moveFloorX.axis = MoveAxis::X;
			moveFloorX.speed = -1.0f;
			moveFloorX.limitDist = 1.5f;
			moveFloorX.port = moveFloor_port;
			AddGameObject<MoveFloor>(moveFloorX);


			MoveFloorDesc moveFloorZ;
			moveFloorZ.pos = Vec3(6.5f, 2.5f, 7.0f);
			moveFloorZ.scale = Vec3(2.0f, 0.1f, 2.0f);
			moveFloorZ.axis = MoveAxis::Z;
			moveFloorZ.speed = 1.0f;
			moveFloorZ.limitDist = 1.5f;
			moveFloorZ.port = moveFloor_port;
			AddGameObject<MoveFloor>(moveFloorZ);

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
