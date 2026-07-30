/*!
@file GameStageBase.cpp
@brief ゲームステージ実体
*/

#include "stdafx.h"
#include "Project.h"
#include "game_controller.h"
#include "GameStageBase.h"
#include "GameProgressManager.h"
namespace basecross {
	//ビューとライトの作成
	void GameStageBase::CreateViewLight() {
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

		//App::GetApp()->RegisterResource(L"DEFAULT_PC_LINE", MeshResource::CreateMeshResource(vertices, indices, false));

	}

	GameStageBase::GameStageBase
	(
		const int& stageNum
	):
		m_StageNum(stageNum),Stage()
	{
	}

	GameStageBase::~GameStageBase()
	{
	}

	void GameStageBase::OnCreate()
	{

		AddGameObject<EffectManager>();

		wstring mediaPath = App::GetApp()->GetDataDirWString();
		EffectManager::g_Instance->RegisterResource(L"ELECTRIC", mediaPath + L"Effects/Electric1.efk");
		wstring texPath = App::GetApp()->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
		App::GetApp()->RegisterTexture(L"BUTTON_AB", texPath + L"Button_AB.png");
		App::GetApp()->RegisterTexture(L"INK_MOZI", texPath + L"Ink_mozi.png");
		//BGM再生
		SoundManager::Get().PlayBGM(L"GAMESTAGE_BGM", 1.0f /*m_BGMVolume*/);


		CreateViewLight();
		StageDateRoad(m_StageNum);

		auto UISprite = AddGameObject<Sprite>(L"BUTTON_AB", Vec3(630, -380, 0), Vec2(971, 600) * 0.25f, Anchor::BottomRight);
		auto inkprite = AddGameObject<Sprite>(L"INK_MOZI", Vec3(-550, 380, 0), Vec2(100, 30), Anchor::Center);
		//UI作成
		auto gaugeBack = AddGameObject<GaugeBack>();
		m_inkGauge = AddGameObject<InkGauge>();

		//ポーズメニュー作成
		m_pauseMenu = ObjectFactory::Create<PauseMenu>(GetThis<Stage>());
		SetSharedGameObject(L"Pause", m_pauseMenu);

		//設定メニュー作成
		m_optionMenu = ObjectFactory::Create<OptionMenu>(GetThis<Stage>());
		SetSharedGameObject(L"Option", m_optionMenu);

		//スカイボックス
		AddGameObject<SkyCube>(L"SKYBOX");

	}

	void GameStageBase::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();
		auto device = App::GetApp()->GetInputDevice();
		auto& pad = device.GetControlerVec()[0];
		GameController::Update();

		auto camera = GetView()->GetTargetCamera();
		auto mainCamera = dynamic_pointer_cast<MainCamera>(camera);

		bool cameraAnimation = mainCamera->GetAnimationFlag();
		bool pause = m_pauseMenu->GetPause();

		bool goalFlag = m_goalPtr->GetGoal();

		if (cameraAnimation || goalFlag)
		{
			//カメラ演出中もしくはゴールしたときはポーズ
			//Pause(true);
			auto objs = GetGameObjectVec();
			for (auto& obj : objs)
			{
				//ゴール関係はポーズしない
				if(dynamic_pointer_cast<Goal>(obj) || 
					dynamic_pointer_cast<GoalEffect>(obj) ||
					dynamic_pointer_cast<GoalParticle>(obj))
				{
					obj->SetUpdateActive(true);
				}
				else
				{
					obj->SetUpdateActive(false);
				}
			}
			return;
		}
		else
		{
			//ポーズ画面が開いていない時はポーズしない
			if (!pause)
			{
				Pause(false);
			}
		}

		
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
		if (IsOption())
		{
			m_optionMenu->OnUpdate();
		}

		if (GameController::IsPressed_ButtonRight() && GameController::IsPressed_DpadLeft()) {
			auto player = GetSharedGameObject<Player>(L"Player", false);
			if (!player) return;
			Vec3 playerPosition = player->GetComponent<Transform>()->GetPosition();
			for (auto& floor : GetGameObjectVec()) {
				auto draw = floor->GetComponent<InkDrawComp>(false);
				if (!draw) continue;
				Vec3 hitPos;
				TRIANGLE tempTri;
				size_t temp;
				if (draw->HitTestStaticMeshSphereTriangles(
					SPHERE(playerPosition, 1.0f),
					SPHERE(playerPosition + Vec3(0.0f, 1.0f, 0.0f), 1.0f),
					hitPos, tempTri, temp)) {

					draw->WriteToWIC();
				}
			}
			
		}
	}

	void GameStageBase::OnDraw()
	{
		if (IsPause())
		{
			m_pauseMenu->OnDraw();
		}
		if (IsOption())
		{
			m_optionMenu->OnDraw();
		}
	}

	void GameStageBase::SetStageNum(int num)
	{
		m_StageNum = num;
	}

	int GameStageBase::GetStageNum()
	{
		return m_StageNum;
	}

	void GameStageBase::StageDateRoad(int num)
	{
		wstring path = App::GetApp()->GetDataDirWString() + L"StageDatas/" + L"StageData" + to_wstring(num) + L".json";

		Json json;
		json.Load(path);
		auto mapData = json.At<JsonArray>(L"Gameobj")->GetObjectArray();

		for (auto& date : mapData)
		{
			auto objTypePtr = date->At<JsonString>(L"type")->GetValue();
			const wstring& objType = objTypePtr;

			switch (ToStageObjType(objType))
			{
			default:
				break;
			case ENUM_ObjType::T_Box: case ENUM_ObjType::T_Floor:
				AddStaticObj(StaticParams(*date));
				break;
			case ENUM_ObjType::T_Player:
				AddPlayerObj(PlayerParams(*date));
				break;
			case ENUM_ObjType::T_Port:
				AddPortObj(ElectricObjBaseParams(*date));
				break;
			case ENUM_ObjType::T_Goal:
				AddGoalObj(ElectricObjBaseParams(*date));
				break;
			case ENUM_ObjType::T_MoveFloor:
				AddMoveFloorObj(MoveFloorParams(*date));
				break;
			case ENUM_ObjType::T_PowerSupply:
				AddPowerSupplyObj(StaticParams(*date));
				break;
			case ENUM_ObjType::T_TrapDoor:
				AddTrapDoorObj(TrapDoorAxisParams(*date));
				break;
			case ENUM_ObjType::T_BeltConveyor:
				AddBeltConveyorObj(ElectricObjBaseParams(*date));
				break;
			case ENUM_ObjType::T_GoalDoor:
				AddGoalDoorObj(GoalDoorParams(*date));
				break;
			case ENUM_ObjType::T_Spring:
				AddSpringObj(SpringParams(*date));
				break;
			}
		}
	}

	GameStageBase::ENUM_ObjType GameStageBase::ToStageObjType(const wstring& objType)
	{
		if (objType == L"Box")  return ENUM_ObjType::T_Box;
		if (objType == L"Player")  return ENUM_ObjType::T_Player;
		if (objType == L"Floor") return ENUM_ObjType::T_Floor;
		if (objType == L"Wall")  return ENUM_ObjType::T_Wall;
		if (objType == L"Port")  return ENUM_ObjType::T_Port;
		if (objType == L"Goal")  return ENUM_ObjType::T_Goal;
		if (objType == L"MoveFloor")  return ENUM_ObjType::T_MoveFloor;
		if (objType == L"PowerSupply")  return ENUM_ObjType::T_PowerSupply;
		if (objType == L"TrapDoorAxis")  return ENUM_ObjType::T_TrapDoor;
		if (objType == L"BeltConveyor")  return ENUM_ObjType::T_BeltConveyor;
		if (objType == L"GoalDoor") return ENUM_ObjType::T_GoalDoor;
		if (objType == L"Spring") return ENUM_ObjType::T_Spring;
		return ENUM_ObjType::T_Unknown;
	}

	void GameStageBase::BaseParams(JsonObject& json, STRUCT_BaseParams& params)
	{
		params.ObjType = json.At<JsonString>(L"type")->GetValue();

		auto posParams = json.At<JsonObject>(L"position");
		params.Pos.x = posParams->At<JsonNumber>(L"x")->GetFloatValue();
		params.Pos.y = posParams->At<JsonNumber>(L"y")->GetFloatValue();
		params.Pos.z = posParams->At<JsonNumber>(L"z")->GetFloatValue();

		auto scaleParams = json.At<JsonObject>(L"scale");
		params.Scale.x = scaleParams->At<JsonNumber>(L"x")->GetFloatValue();
		params.Scale.y = scaleParams->At<JsonNumber>(L"y")->GetFloatValue();
		params.Scale.z = scaleParams->At<JsonNumber>(L"z")->GetFloatValue();

		auto rotParams = json.At<JsonObject>(L"rotation");
		params.Rot.x = rotParams->At<JsonNumber>(L"x")->GetFloatValue();
		params.Rot.y = rotParams->At<JsonNumber>(L"y")->GetFloatValue();
		params.Rot.z = rotParams->At<JsonNumber>(L"z")->GetFloatValue();
	}

	GameStageBase::STRUCT_BaseParams GameStageBase::StaticParams(JsonObject& json)
	{
		STRUCT_BaseParams params;
		BaseParams(json, params);
		return params;
	}

	GameStageBase::STRUCT_PlayerParams GameStageBase::PlayerParams(JsonObject& json)
	{
		STRUCT_PlayerParams params;
		auto childObjectData = json.At<JsonObject>(L"childObjectData");
		params.InkMax = childObjectData->At<JsonNumber>(L"InkMax")->GetFloatValue();
		BaseParams(json, params.StageObjParams);
		return params;
	}

	GameStageBase::STRUCT_ElectricObjBaseParams GameStageBase::ElectricObjBaseParams(JsonObject& json)
	{
		STRUCT_ElectricObjBaseParams params;
		BaseParams(json, params.StageObjParams);
		auto childObjectData = json.At<JsonObject>(L"childObjectData");
		params.PortID = childObjectData->At<JsonNumber>(L"PortID")->GetIntValue();
		return params;
	}

	GameStageBase::STRUCT_MoveFloorParams GameStageBase::MoveFloorParams(JsonObject& json)
	{
		STRUCT_MoveFloorParams params;
		BaseParams(json, params.StageObjParams);

		auto childObjectData = json.At<JsonObject>(L"childObjectData");
		float axisStr = childObjectData->At<JsonNumber>(L"Axis")->GetFloatValue();
		switch ((int)axisStr)
		{
		default:
			break;
		case 0:
			params.Axis = MoveAxis::X;
			break;
		case 1:
			params.Axis = MoveAxis::Y;
			break;
		case 2:
			params.Axis = MoveAxis::Z;
			break;
		}

		params.Speed = childObjectData->At<JsonNumber>(L"Speed")->GetFloatValue();
		params.LimitDist = childObjectData->At<JsonNumber>(L"LimitDistance")->GetFloatValue();
		params.PortID = childObjectData->At<JsonNumber>(L"PortID")->GetIntValue();

		return params;
	}

	GameStageBase::STRUCT_TrapDoorAxis GameStageBase::TrapDoorAxisParams(JsonObject& json)
	{
		STRUCT_TrapDoorAxis params;
		BaseParams(json, params.StageObjParams);
		auto childObjectData = json.At<JsonObject>(L"childObjectData");
		float axisStr = childObjectData->At<JsonNumber>(L"Axis")->GetFloatValue();
		switch ((int)axisStr)
		{
		default:
			break;
		case 0:
			params.Axis = MoveAxis::X;
			break;
		case 1:
			params.Axis = MoveAxis::Y;
			break;
		case 2:
			params.Axis = MoveAxis::Z;
			break;
		}
		params.Speed = childObjectData->At<JsonNumber>(L"Speed")->GetFloatValue();
		params.PortID = childObjectData->At<JsonNumber>(L"PortID")->GetIntValue();
		return params;
	}

	GameStageBase::STRUCT_GoalDoorParams GameStageBase::GoalDoorParams(JsonObject& json)
	{
		STRUCT_GoalDoorParams params;
		BaseParams(json, params.StageObjParams);

		auto childObjectData = json.At<JsonObject>(L"childObjectData");
		auto sideData = childObjectData->At<JsonNumber>(L"Side");
		float sideStr = -1;
		if (sideData) {
			sideStr = sideData->GetFloatValue();
		}
		switch ((int)sideStr)
		{
		default:
			params.Side = DoorSide::Left;
			break;
		case 0:
			params.Side = DoorSide::Left;
			break;
		case 1:
			params.Side = DoorSide::Right;
			break;
		}
		params.PortID = childObjectData->At<JsonNumber>(L"PortID")->GetIntValue();

		return params;
	}

	GameStageBase::STRUCT_SpringParams GameStageBase::SpringParams(JsonObject& json)
	{
		STRUCT_SpringParams params;
		BaseParams(json, params.StageObjParams);
		auto childObjectData = json.At<JsonObject>(L"childObjectData");
		auto moveDir = childObjectData->At<JsonObject>(L"MoveDir");
		params.MoveDir.x = moveDir->At<JsonNumber>(L"x")->GetFloatValue();
		params.MoveDir.y = moveDir->At<JsonNumber>(L"y")->GetFloatValue();
		params.MoveDir.z = moveDir->At<JsonNumber>(L"z")->GetFloatValue();
		return params;
	}

	void GameStageBase::AddStaticObj(STRUCT_BaseParams params)
	{
		switch (ToStageObjType(params.ObjType))
		{
		default:
			break;
		case ENUM_ObjType::T_Floor:
			AddGameObject<Floor>(params.Scale, params.Rot, params.Pos);
			break;
		case ENUM_ObjType::T_Box:
			AddGameObject<MoveObj>(params.Scale, params.Rot, params.Pos);
			break;
		}
	}

	void GameStageBase::AddPlayerObj(STRUCT_PlayerParams params)
	{
		auto playerPtr = AddGameObject<Player>(params.StageObjParams.Scale, params.StageObjParams.Rot, params.StageObjParams.Pos, params.InkMax);
		SetSharedGameObject(L"player", playerPtr);

		auto view = GetView();
		auto camera = view->GetTargetCamera();
		auto mainCamera = dynamic_pointer_cast<MainCamera>(camera);
		mainCamera->SetTarget(playerPtr);
	}

	void GameStageBase::AddPortObj(STRUCT_ElectricObjBaseParams params)
	{
		auto portPtr = AddGameObject<Port>(params.StageObjParams.Scale, params.StageObjParams.Rot, params.StageObjParams.Pos);
		Map_Ports[params.PortID] = portPtr;
	}


	void GameStageBase::AddGoalObj(STRUCT_ElectricObjBaseParams params)
	{
		m_goalPtr = AddGameObject<Goal>(params.StageObjParams.Scale, params.StageObjParams.Rot, params.StageObjParams.Pos, Map_Ports[params.PortID]);

		auto view = GetView();
		auto camera = view->GetTargetCamera();
		auto mainCamera = dynamic_pointer_cast<MainCamera>(camera);
		mainCamera->SetGoal(m_goalPtr);
	}

	void GameStageBase::AddMoveFloorObj(STRUCT_MoveFloorParams params)
	{
		MoveFloorDesc desc;
		desc.axis = params.Axis;
		desc.limitDist = params.LimitDist;
		desc.speed = params.Speed;

		if (params.PortID==-1)
		{
			desc.port = nullptr;
		}
		else
		{
			desc.port = Map_Ports[params.PortID];
		}

		AddGameObject<MoveFloor>(params.StageObjParams.Scale, params.StageObjParams.Rot, params.StageObjParams.Pos, desc);
	}

	void GameStageBase::AddPowerSupplyObj(STRUCT_BaseParams params)
	{
		AddGameObject<PowerSupply>(params.Scale, params.Rot, params.Pos);
	}

	void GameStageBase::AddTrapDoorObj(STRUCT_TrapDoorAxis params)
	{
		TrapDoorAxisDesc desc;
		desc.axis = params.Axis;
		desc.speed = params.Speed;
		desc.port = Map_Ports[params.PortID];

		AddGameObject<TrapDoorB>(params.StageObjParams.Scale, params.StageObjParams.Rot, params.StageObjParams.Pos, desc);
	}

	void GameStageBase::AddBeltConveyorObj(STRUCT_ElectricObjBaseParams params)
	{
		STRUCT_ElectricObjBaseParams desc;
		AddGameObject<BeltConveyor>(params.StageObjParams.Scale, params.StageObjParams.Rot, params.StageObjParams.Pos, Map_Ports[params.PortID]);
	}

	void GameStageBase::AddGoalDoorObj(STRUCT_GoalDoorParams params)
	{
		STRUCT_GoalDoorParams desc;
		AddGameObject<GoalDoor>(params.StageObjParams.Scale, params.StageObjParams.Rot, params.StageObjParams.Pos, Map_Ports[params.PortID], params.Side);
	}

	void GameStageBase::AddSpringObj(STRUCT_SpringParams params)
	{
		AddGameObject<ObstacleSpring>(params.StageObjParams.Scale, params.StageObjParams.Rot, params.StageObjParams.Pos, params.MoveDir);
	}

	bool GameStageBase::IsPause() const
	{
		return m_pauseMenu->GetPause() && m_pauseMenu;
	}

	void GameStageBase::Pause(bool isPause)
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

	bool GameStageBase::IsOption() const
	{
		return m_optionMenu->GetOption() && m_optionMenu;
	}

	void GameStageBase::Option(bool isOption)
	{
		bool option = m_optionMenu->GetOption();
		m_isOption = isOption;

		auto objs = GetGameObjectVec();
		auto view = GetView();
		auto camera = view->GetTargetCamera();
		for (auto& obj : objs)
		{
			obj->SetUpdateActive(!m_isOption);
		}
		//EffectManager::g_Instance->OnDraw();
	}


}
//end basecross