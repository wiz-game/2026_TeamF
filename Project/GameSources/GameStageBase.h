///*!
//@file GameStage.h
//@brief ゲームステージ
//*/
//
#pragma once
#include "stdafx.h"
#include "Json.h"

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
			T_Floor,
			T_Wall,
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

		//テスト用のパラメータ
		//struct SUTRUCT_BoxParams
		//{
		//	int HP;
		//	int MaxHP;
		//};

		//構築と破棄
		GameStageBase() :Stage(){}
		virtual ~GameStageBase() {}
		
		void CreateViewLight();

		void OnCreate();

		void OnUpdate();

		void StageDateRoad(int num);

		//オブジェクトタイプをwstringからENUMに変換
		GameStageBase::ENUM_ObjType ToStageObjType(const wstring& objType);

		//基本のパラメーターの取得
		void BaseParams(JsonObject& json, STRUCT_BaseParams& params);

		//基本情報しかないオブジェクトのパラメータの取得
		GameStageBase::STRUCT_BaseParams StaticParams(JsonObject& json);

		//ボックスのパラメータの取得
		//SUTRUCT_BoxParams BoxParams(JsonObject& json, STRUCT_BaseParams params);

		//ゲームオブジェクトの生成
		void AddStaticObj(STRUCT_BaseParams params);

	};
}
//end basecross
