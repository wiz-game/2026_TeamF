#pragma once
#include "stdafx.h"
#include "Player.h";

namespace basecross
{
	class InkGauge:public GameObject
	{
		std::shared_ptr<PCTSpriteDraw> m_draw;
		std::shared_ptr<Transform> m_trans;

		std::shared_ptr<Player> m_player;

		std::vector<VertexPositionColorTexture> m_vertices;
		std::vector<uint16_t> m_indices;

		Col4 m_color = Col4(0, 0, 0, 1);

		float m_currentInk;
		float m_maxInk;
		float m_ink;

		float m_width;
		float m_height;
		float m_unitWidth;	//インク1あたりの横幅

		Vec2 m_offsetPos;

	public:
		InkGauge(const std::shared_ptr<Stage>stage) :
			GameObject(stage),
			m_currentInk(10.0f),
			m_maxInk(15.0f),
			m_ink(15.0f),
			m_width(0.0f),
			m_height(0.0f),
			m_unitWidth(1.0f),
			m_offsetPos(Vec2(-100.0f,-15.0f))
		{
		}

		~InkGauge()
		{
		}

		void OnCreate()override;
		void OnUpdate()override;
	};
}