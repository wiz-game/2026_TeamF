/*!
@file Player.h
@brief プロトタイプ用地面
*/

#pragma once
#include "stdafx.h"
#include "PNTDXModelDraw.h"


namespace basecross
{
	class Floor : public GameObject
	{
		std::shared_ptr<PNTStaticDraw> m_draw;
		std::shared_ptr<Transform> m_trans;

		Vec3 m_pos;
		Vec3 m_scale;
	public:
		Floor(const std::shared_ptr<Stage> stage) :
			GameObject(stage),
			m_pos(Vec3(0, 0, 0)),
			m_scale(Vec3(8, 1, 15))
		{

		}

		virtual ~Floor()
		{

		}

		void OnCreate();
		void OnUpdate();
	};
}