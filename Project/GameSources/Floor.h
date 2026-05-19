/*!
@file Player.h
@brief プロトタイプ用地面
*/

#pragma once
#include "stdafx.h"
#include "PNTDXModelDraw.h"


namespace basecross
{
	class Floor : public StageObjBase
	{
	public:

		Floor
		(
			const shared_ptr<Stage>& StagePtr,
			const Vec3& Scale,
			const Vec3& Rot,
			const Vec3& Pos
		);

		virtual ~Floor(){}

		std::shared_ptr<PNTStaticDraw> m_draw;
		std::shared_ptr<Transform> m_trans;

		Vec3 m_pos;
		Vec3 m_scale;


		void OnCreate() override;	//初期設定用
		void OnUpdate() override;	//更新用
	};
}