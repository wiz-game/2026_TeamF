/*!
@file Foo.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	class GenericSprite : public GameObject;
	//--------------------------------------------------------------------------------------
	class Box : public StageObjBase
	{
	public:
		// 構築と破棄
		Box(
			const shared_ptr<Stage>& StagePtr,
			const Vec3& Scale,
			const Vec3& Rot,
			const Vec3& Pos
		) :
			StageObjBase(StagePtr, Scale, Rot, Pos),
			m_pos(Pos),
			m_scale(Scale),
			m_rot(Rot)
		{
		}

		virtual ~Box()
		{
		}

		Vec3 m_pos;
		Vec3 m_scale;
		Vec3 m_rot;

		shared_ptr<PNTStaticDraw> m_draw;
		shared_ptr<Transform> m_trans;


		virtual void OnCreate() override; // 初期化
		//virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画
	};

}
//end basecross
