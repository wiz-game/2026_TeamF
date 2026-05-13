/*!
@file Goal.h
@brief ゴール
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	//--------------------------------------------------------------------------------------
	//	class Goal : public GameObject;
	//--------------------------------------------------------------------------------------
	class Goal : public StageObjBase
	{
		std::shared_ptr<Transform> m_transform;
		std::shared_ptr<PNTDXModelDraw> m_draw;
		std::shared_ptr<PNTStaticDraw> m_staticDraw;

		std::shared_ptr<Player> m_player;
		std::shared_ptr<Port> m_port = nullptr;

	public:
		// 構築と破棄
		Goal(
			const shared_ptr<Stage>& StagePtr,
			const Vec3& Scale,
			const Vec3& Rot,
			const Vec3& Pos,
			std::shared_ptr<Port> port
		) :
			StageObjBase(StagePtr, Scale, Rot, Pos),
			m_port(port)
		{
		}
		virtual ~Goal()
		{
		}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画
	};

}
//end basecross
