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
		camera->SetEye(Vec3(0.0f, 90.0f, 1.0f));
		camera->SetAt(Vec3(0.0f, 10.0f, 0.0f));

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

			AddGameObject<EffectManager>();

			wstring mediaPath = App::GetApp()->GetDataDirWString();
			EffectManager::g_Instance->RegisterResource(L"ELECTRIC", mediaPath + L"Effects/Electric1.efk");

			wstring texPath = app->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
			app->RegisterTexture(L"BUTTON_AB", texPath + L"Button_AB.png");
			app->RegisterTexture(L"INK_MOZI", texPath + L"Ink_mozi.png");


			//プレイヤー作成
			m_Player = AddGameObject<Player>(Vec3(0,1,0),Vec3(), Vec3(),float(20.0f));
			SetSharedGameObject(L"player", m_Player);

			//カメラ取得
			auto view = GetView();
			auto camera = view->GetTargetCamera();
			auto mainCamera = dynamic_pointer_cast<MainCamera>(camera);
			mainCamera->SetTarget(m_Player);

			//BGM再生
			if(!m_stageBGM)
				m_stageBGM = SoundManager::Get().PlayBGM(L"STAGE", 0.55f);

			//UI作成
			auto gaugeBack = AddGameObject<GaugeBack>();
			auto gauge = AddGameObject<InkGauge>();

			//ポーズメニュー作成
			m_pauseMenu = ObjectFactory::Create<PauseMenu>(GetThis<Stage>());

			//プロトタイプ用地面作成
			JPH::StaticCompoundShapeSettings compoundSettings;
			JPH::BoxShapeSettings floorShapeSettings(JPH::Vec3(8.0f, 1.0f, 15.0f) * 0.45f);
			JPH::ShapeRefC floorShape = floorShapeSettings.Create().Get();
			compoundSettings.AddShape(JPH::Vec3(0.0f, -1.0f, 0.0f), JPH::Quat::sIdentity(), floorShape);

			auto level = AddGameObject<GameObject>();
			auto rb = level->AddComponent<JoltRigidBody>();

			JoltRigidBody::Settings settings;
			settings.shape = floorShape;
			settings.motionType = JPH::EMotionType::Static;
			settings.objectLayer = Layers::NON_MOVING;

			rb->Initialize(settings);


			//AddGameObject<Floor>(Vec3(5,0,5), Vec3(), Vec3());

			//AddGameObject<PowerSupply>();
			//AddGameObject<Port>();
			//AddGameObject<Goal>();
			//AddGameObject<UpDownFloor>();

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

			//地面					scale						position
			AddGameObject<Floor>(Vec3(8, 1, 8), Vec3(0), Vec3(0.0f, -1.0f, 3.5f) );
			AddGameObject<Floor>(Vec3(8, 1, 8), Vec3(0), Vec3(0.0f, -1.0f, -4.5f));

			//AddGameObject<Floor>( Vec3(8.0f, 0.5f, 10.0f), Vec3(0),Vec3(10.0f, 2.0f, 15.0f));
			//AddGameObject<Floor>( Vec3(8.0f, 0.5f, 10.0f), Vec3(0), Vec3(10.0f, 2.0f, 29.0f));
			AddGameObject<Floor>(Vec3(8.0f, 0.5f, 15.0f), Vec3(0), Vec3(0.0f, -1.0f, 26.0f));


			AddGameObject<PowerSupply>(Vec3(1.0f, 0.5f, 1.0f), Vec3(0), Vec3(0.0f, -0.3f, -4.0f));
			//AddGameObject<PowerSupply>(Vec3(1.0f, 0.1f, 1.0f), Vec3(0), Vec3(10.0f, 2.4f, 11.0f));

			//port										scale				rotation		position
			auto moveFloor_port = AddGameObject<Port>(Vec3(1.0f, 0.5f, 1.0f), Vec3(0), Vec3(0.0f, -0.3f, 3.0f));
			//auto trapDoor_port = AddGameObject<Port>(Vec3(1.0f, 0.1f, 1.0f), Vec3(0), Vec3(10.0f, 2.4f, 15.0f));
			//auto goal_port = AddGameObject<Port>(Vec3(1.0f, 0.1f, 1.0f), Vec3(0), Vec3(10.0f, 2.4f, 25.0f));

			//Goal					scale			rotation		position			portの指定(nullptrの場合最初から表示)
			AddGameObject<Goal>(Vec3(3.0f,3.0f,0.5f), Vec3(0), Vec3(0.0f, 1.0f, 33.0f), nullptr);

			
			//左に開く扉
			//auto leftDoor = AddGameObject<GoalDoor>(
			//	Vec3(4.0f, 5.0f, 1.0f), Vec3(0), Vec3(8.0f, 4.0f, 30.0f), //Scale, Rotation, Position
			//	goal_port, 
			//	Vec3(-1, 0, 0));//移動する方向を指定。
			////右に開く扉
			//auto rightDoor = AddGameObject<GoalDoor>(
			//	Vec3(4.0f, 5.0f, 1.0f), Vec3(0), Vec3(12.0f, 4.0f, 30.0f), 
			//	goal_port,
			//	Vec3(1, 0, 0));

			//トラップドアの初期設定
			TrapDoorAxisDesc moveDoorX;
			moveDoorX.axis = MoveAxis::X;
			moveDoorX.speed = -0.01f;
			moveDoorX.port = nullptr;
			//moveDoorX.bReverseConnect = false;
			AddGameObject<TrapDoorAxis>(Vec3(2.0f, 0.1f, 4.0f), Vec3(XM_PIDIV2, 0.0f, 0.0f), Vec3(5.0f, -0.5f, 0.0f), moveDoorX);

			//動く床の初期設定
			//MoveFloorDesc moveFloorY;
			//moveFloorY.axis = MoveAxis::Y;		//移動する軸の指定
			//moveFloorY.speed = 1.0f;			//移動速度
			//moveFloorY.limitDist = 3.0f;		//移動上限
			//moveFloorY.port = moveFloor_port;	//portの指定
			//AddGameObject<MoveFloor>(Vec3(2.0f, 0.1f, 2.0f), Vec3(0),Vec3(0.0f, -0.5f, 5.6f),  moveFloorY);

			//MoveFloorDesc moveFloorX;
			//moveFloorX.axis = MoveAxis::X;
			//moveFloorX.speed = 1.0f;
			//moveFloorX.limitDist = -3.0f;
			//moveFloorX.port = moveFloor_port;
			//AddGameObject<MoveFloor>(Vec3(2.0f, 0.1f, 2.0f), Vec3(0),Vec3(5.0f, 2.5f, 5.5f), moveFloorX);

			//MoveFloorDesc moveFloorZ;
			//moveFloorZ.axis = MoveAxis::Z;
			//moveFloorZ.speed = -1.0f;
			//moveFloorZ.limitDist = 3.0f;
			//moveFloorZ.port = moveFloor_port;
			//AddGameObject<MoveFloor>(Vec3(2.0f, 0.1f, 2.0f), Vec3(0),Vec3(7.0f, 2.5f,5.5f),  moveFloorZ);

			//									Scale			Rotation		Position			portの指定
			AddGameObject<BeltConveyor>(Vec3(2.0f, 0.1f, 5.0f), Vec3(0,0,0), Vec3(0.0f, -0.7f, 16.0f), nullptr);
			AddGameObject<BeltConveyor>(Vec3(2.0f, 0.1f, 8.0f), Vec3(0, -XM_PIDIV2, 0), Vec3(0.0f, -0.7f, 22.0f), nullptr);
			AddGameObject<BeltConveyor>(Vec3(2.0f, 0.1f, 8.0f), Vec3(0, XM_PIDIV2, 0), Vec3(0.0f, -0.7f, 25.0f), nullptr);

			MoveFloorDesc moveFloorX_2;
			moveFloorX_2.axis = MoveAxis::X;	//移動する軸の指定
			moveFloorX_2.speed = 1.0f;			//移動速度
			moveFloorX_2.limitDist = 6.0f;		//移動上限
			moveFloorX_2.port = nullptr;	//portの指定

			AddGameObject<MoveFloor>(Vec3(2.0f, 0.1f, 2.0f), Vec3(0), Vec3(-3.0f, -0.5f, 8.5f), moveFloorX_2);

			MoveFloorDesc moveFloorX_3;
			moveFloorX_3.axis = MoveAxis::X;	//移動する軸の指定
			moveFloorX_3.speed = 1.0f;			//移動速度
			moveFloorX_3.limitDist = -6.0f;		//移動上限
			moveFloorX_3.port = nullptr;	//portの指定

			AddGameObject<MoveFloor>(Vec3(2.0f, 0.1f, 2.0f), Vec3(0), Vec3(3.0f, -0.5f, 10.5f), moveFloorX_3);

			MoveFloorDesc moveFloorX_4;
			moveFloorX_4.axis = MoveAxis::X;	//移動する軸の指定
			moveFloorX_4.speed = 1.0f;			//移動速度
			moveFloorX_4.limitDist = 6.0f;		//移動上限
			moveFloorX_4.port = nullptr;	//portの指定

			AddGameObject<MoveFloor>(Vec3(2.0f, 0.1f, 2.0f), Vec3(0), Vec3(-3.0f, -0.5f, 12.5f), moveFloorX_4);

			auto UISprite = AddGameObject<Sprite>(L"BUTTON_AB", Vec3(630,-380,0), Vec2(250,200), Anchor::BottomRight);
			auto inkprite = AddGameObject<Sprite>(L"INK_MOZI", Vec3(-550, 380, 0), Vec2(100, 30), Anchor::Center);

			//スカイボックス
			AddGameObject<SkyCube>(L"SKYBOX");
			
		}
		catch (...) {
			throw;
		}
	}

	void ProtoStage::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();
		auto device = App::GetApp()->GetInputDevice();
		auto& pad = device.GetControlerVec()[0];
		GameController::Update();

		bool pause = m_pauseMenu->GetPause();
		//m_isPause = pause;

		if (pad.wPressedButtons & XINPUT_GAMEPAD_START)
		{
			m_pauseMenu->SetPause(!pause);
			Pause(!pause);
			//BGMを中断
			SoundManager::Get().PauseBGM(!pause);
		}

		if (IsPause())
		{
			m_pauseMenu->OnUpdate();
		}
	}

	void ProtoStage::OnUpdate2()
	{

	}

	void ProtoStage::OnDraw()
	{
		if (IsPause())
		{
			m_pauseMenu->OnDraw();
		}
	}

	bool ProtoStage::IsPause() const
	{
		return m_pauseMenu->GetPause() && m_pauseMenu;
	}

	void ProtoStage::Pause(bool isPause)
	{
		bool pause = m_pauseMenu->GetPause();
		m_isPause = isPause;

		auto objs = GetGameObjectVec();
		auto view = GetView();
		auto camera = view->GetTargetCamera();
		for (auto& obj : objs)
		{
			obj->SetUpdateActive(!m_isPause);
		}
		//EffectManager::g_Instance->OnDraw();
	}
}
//end basecross
