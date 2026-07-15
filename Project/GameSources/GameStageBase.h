///*!
//@file GameStage.h
//@brief ゲームステージ
//*/
//
#pragma once
#include "stdafx.h"
#include "Json.h"
#include "PauseMenu.h"

#include <unordered_set>
#include <string>

namespace basecross {
	
	//--------------------------------------------------------------------------------------
	//	ゲームステージクラス
	//--------------------------------------------------------------------------------------
	class GameStageBase : public Stage
	{
	public:
		//オブジェクトの種類
		enum ENUM_ObjType
		{
			T_Box,
			T_Player,
			T_Floor,
			T_Wall,
			T_Goal,
			T_Port,	
			T_MoveFloor,
			T_PowerSupply,
			T_TrapDoor,
			T_BeltConveyor,
			T_GoalDoor,
			T_Spring,
			T_Unknown,
		};

		//基本のパラメーター
		struct STRUCT_BaseParams
		{
			wstring ObjType;
			Vec3 Pos;
			Vec3 Rot;
			Vec3 Scale;
		};

		//Playerのパラメーター
		struct STRUCT_PlayerParams
		{
			STRUCT_BaseParams StageObjParams;
			float InkMax;
		};

		//portが接続できるオブジェクトの基本のパラメーター
		struct STRUCT_ElectricObjBaseParams
		{
			STRUCT_BaseParams StageObjParams;
			int PortID;
		};

		//上下する床のパラメーター
		struct STRUCT_MoveFloorParams
		{
			STRUCT_BaseParams StageObjParams;
			MoveAxis Axis;
			float Speed;
			float LimitDist;
			int PortID;
		};

		//トラップドアのパラメーター
		struct STRUCT_TrapDoorAxis
		{
			STRUCT_BaseParams StageObjParams;
			MoveAxis Axis;
			float Speed;
			int PortID;
		};

		//ゴールの扉のパラメーター
		struct STRUCT_GoalDoorParams
		{
			STRUCT_BaseParams StageObjParams;
			Vec3 MoveDir;
			int PortID;
		};

		//バネのパラメーター
		struct STRUCT_SpringParams
		{
			STRUCT_BaseParams StageObjParams;
			Vec3 MoveDir;
		};

		//テスト用のパラメータ
		//struct SUTRUCT_BoxParams
		//{
		//	int HP;
		//	int MaxHP;
		//};

		map<int, shared_ptr<Port>>Map_Ports;

		////ステージ番号
		int m_StageNum = 11;	

		//ポーズメニュー
		std::shared_ptr<PauseMenu> m_pauseMenu;	
		std::shared_ptr<OptionMenu> m_optionMenu;
		bool m_isPause = false;
		bool m_isOption = false;


		//構築と破棄
		GameStageBase() :Stage(){}
		GameStageBase(const int& num);
		virtual ~GameStageBase();
		void CreateViewLight();
		void OnCreate();
		void OnUpdate();
		void OnDraw();
		void SetStageNum(int num);
		int GetStageNum();
		void StageDateRoad(int num);

		//オブジェクトタイプをwstringからENUMに変換
		GameStageBase::ENUM_ObjType ToStageObjType(const wstring& objType);

		//基本のパラメーターの取得
		void BaseParams(JsonObject& json, STRUCT_BaseParams& params);

		//基本情報しかないオブジェクトのパラメータの取得
		GameStageBase::STRUCT_BaseParams StaticParams(JsonObject& json);

		//プレイヤーのパラメーターの取得
		GameStageBase::STRUCT_PlayerParams PlayerParams(JsonObject& json);

		//portが接続できるオブジェクトの基本のパラメーターの取得
		GameStageBase::STRUCT_ElectricObjBaseParams ElectricObjBaseParams(JsonObject& json);

		//上下する床のパラメーターの取得
		GameStageBase::STRUCT_MoveFloorParams MoveFloorParams(JsonObject& json);

		//トラップドアのパラメーターの取得
		GameStageBase::STRUCT_TrapDoorAxis TrapDoorAxisParams(JsonObject& json);

		//ゴールの扉のパラメーターの取得
		GameStageBase::STRUCT_GoalDoorParams GoalDoorParams(JsonObject& json);

		//バネのパラメーターの取得
		GameStageBase::STRUCT_SpringParams SpringParams(JsonObject& json);

		//ボックスのパラメータの取得
		//SUTRUCT_BoxParams BoxParams(JsonObject& json, STRUCT_BaseParams params);

		//ゲームオブジェクトの生成
		void AddStaticObj(STRUCT_BaseParams params);

		//プレイヤーオブジェクトの生成
		void AddPlayerObj(STRUCT_PlayerParams params);

		//ポートオブジェクトの生成
		void AddPortObj(STRUCT_ElectricObjBaseParams params);

		//ゴールオブジェクトの生成
		void AddGoalObj(STRUCT_ElectricObjBaseParams params);

		//上下する床オブジェクトの生成
		void AddMoveFloorObj(STRUCT_MoveFloorParams params);

		//電源オブジェクトの生成
		void AddPowerSupplyObj(STRUCT_BaseParams params);

		//トラップドアオブジェクトの生成
		void AddTrapDoorObj(STRUCT_TrapDoorAxis params);

		//ベルトコンベアオブジェクトの生成
		void AddBeltConveyorObj(STRUCT_ElectricObjBaseParams params);

		//ゴールの扉オブジェクトの生成
		void AddGoalDoorObj(STRUCT_GoalDoorParams params);

		//バネオブジェクトの生成
		void AddSpringObj(STRUCT_SpringParams params);

		bool IsPause() const;
		void Pause(bool isPause);

		bool IsOption() const;
		void Option(bool isOption);
	};
}
//end basecross
