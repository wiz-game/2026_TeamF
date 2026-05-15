/*!
@file PowerSupply.h
@brief 電源
*/

#pragma once
#include "stdafx.h"
#include "PNTDXModelDraw.h"
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
		PowerSupply(const shared_ptr<Stage>& stage,
			const Vec3& Scale,
			const Vec3& Rot,
			const Vec3& Position) :
			StageObjBase(stage, Scale, Rot, Position)
		{
		}
		virtual ~PowerSupply()
		{
		}

		void OnCreate() override;
		void OnUpdate() override;
		void OnCollisionEnter(std::shared_ptr<GameObject>& obj);	//インクとの当たり判定

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

