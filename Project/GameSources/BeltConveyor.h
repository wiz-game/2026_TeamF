/*!
@file BeltConveyor.h
@brief ベルトコンベアー
*/

#pragma once
#include "stdafx.h"
#include "FloorDecision.h"
#include "BeltConveyorTex.h"

namespace basecross {

	class Player;
	class Port;

	//--------------------------------------------------------------------------------------
	//	class BeltConveyor : public StageObjBase;
	//--------------------------------------------------------------------------------------
	class BeltConveyor : public StageObjBase
	{
		std::shared_ptr<Transform> m_transform;
		std::shared_ptr<PNTDXModelDraw> m_draw;
		std::shared_ptr<PNTStaticDraw> m_staticDraw;
		std::shared_ptr<CollisionObb> m_coll;

		std::shared_ptr<Player> m_player;
		std::shared_ptr<Port> m_port;

		std::shared_ptr<FloorDecision> m_floorDec;
		std::shared_ptr<BeltConveyorTex> m_texObj;//テクスチャオブジェクト

		float m_speed = 2.0f;
		float m_limitDist;

		bool m_isMove = false;
		bool isConnect = false;


	public:
		// 構築と破棄
		BeltConveyor(const shared_ptr<Stage>& stage,
			const Vec3& Scale,
			const Vec3& Rot,
			const Vec3& Position,
			shared_ptr<Port> port) :
			StageObjBase(stage,Scale,Rot,Position),
			m_port(port)
		{
		}
		virtual ~BeltConveyor()
		{
		}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

	};
}
//end basecross
