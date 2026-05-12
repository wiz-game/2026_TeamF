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

	void GameStageBase::OnCreate()
	{
		CreateViewLight();
		StageDateRoad(1);

	}

	void GameStageBase::OnUpdate()
	{

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
			case ENUM_ObjType::T_Floor:
					AddStaticObj(StaticParams(*date));
					break;
			}
		}
	}

	GameStageBase::ENUM_ObjType GameStageBase::ToStageObjType(const wstring& objType)
	{
		if (objType == L"Box")  return ENUM_ObjType::T_Box;
		if (objType == L"Floor") return ENUM_ObjType::T_Floor;
		if (objType == L"Wall")  return ENUM_ObjType::T_Wall;

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
		}
	}
}
//end basecross