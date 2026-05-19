/*!
@file PowerSupply.h
@brief 電源
*/

#pragma once
#include "stdafx.h"
#include "PNTDXModelDraw.h"
#include "StageObjBase.h"
#include "Electrified.h"
#include "StageObjBase.h"

namespace basecross {
	class PowerSupply : public StageObjBase
	{
		std::shared_ptr<Transform> m_transform; // トランスフォームはよく使うのでメンバにしておく
		std::shared_ptr<PNTDXModelDraw> m_draw; // ドローコンポーネント
		std::shared_ptr<PNTStaticDraw> m_staticDraw;

		bool isConnect = false;
		bool isPower = true;

	public:
		// ステージを引数にしたコンストラクタ【必須】
		PowerSupply(const std::shared_ptr<Stage>& stage,
			const Vec3& Scale,
			const Vec3& Rot,
			const Vec3& Pos
		) :
			StageObjBase(stage, Scale, Rot,Pos)
		{
		}

		

		void OnCreate() override; // 初期設定用の関数(UnityのStartメソッドに相当)
		void OnUpdate() override; // 毎フレーム実行される関数(UnityのUpdateメソッドに相当)
		void OnCollisionEnter(std::shared_ptr<GameObject>& obj);	//インクとの当たり判定
		void OnCollisionExit(std::shared_ptr<GameObject>& obj);	//インクとの当たり判定

		bool GetConnect() const
		{
			return isConnect;
		}

		bool GetisPower() const
		{
			return isPower;
		}
	};

}
//end basecross

