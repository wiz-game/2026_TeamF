/*!
@file GameStageBase.cpp
@brief ゲームステージ実体
*/

#include "stdafx.h"
#include "Project.h"
#include "game_controller.h"
#include "GameStageBase.h"

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

		App::GetApp()->RegisterResource(L"DEFAULT_PC_LINE", MeshResource::CreateMeshResource(vertices, indices, false));
	}

	void GameStageBase::OnCreate()
	{
		CreateViewLight();
		StageDateRoad(1);

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
			case ENUM_ObjType::T_Box:
				AddGameObject<Box>(StaticParams(*date));
				break;
			case ENUM_ObjType::T_Floor:
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

		auto rotParams = json.At<JsonObject>(L"rotationEuler");
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
	
	//GameStageBase::SUTRUCT_BoxParams GameStageBase::BoxParams(JsonObject& json, STRUCT_BaseParams params)
	//{
	//	SUTRUCT_BoxParams boxParams;
	//	BaseParams(json, params);
	//	boxParams.HP = json.At<JsonNumber>(L"hp")->GetIntValue();
	//	boxParams.MaxHP = json.At<JsonNumber>(L"max_hp")->GetIntValue();
	//	return boxParams;
	//}

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
			AddGameObject<Box>(params.Scale, params.Rot, params.Pos);
			break;
		}
	}

	void GameStageBase::AddPlayerObj(STRUCT_PlayerParams params)
	{
		auto playerPtr = AddGameObject<Player>(params.StageObjParams.Scale, params.StageObjParams.Rot, params.StageObjParams.Pos, params.InkMax);

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
		if (!Map_Ports[params.PortID])return;
		AddGameObject<Goal>(params.StageObjParams.Scale, params.StageObjParams.Rot, params.StageObjParams.Pos, Map_Ports[params.PortID]);
	}

	void GameStageBase::AddMoveFloorObj(STRUCT_MoveFloorParams params)
	{
		MoveFloorDesc desc;
		desc.axis = params.Axis;
		desc.limitDist = params.LimitDist;
		desc.speed = params.Speed;
		desc.port = Map_Ports[params.PortID];

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

		AddGameObject<TrapDoorAxisDesc>(params.StageObjParams.Scale, params.StageObjParams.Rot, params.StageObjParams.Pos, desc);
	}

}
//end basecross