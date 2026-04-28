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
	public:
		InkGauge(const std::shared_ptr<Stage>stage):
			GameObject(stage),
			m_currentInk(10.0f),
			m_maxInk(10.0f)
		{
		}

		~InkGauge()
		{
		}

		void OnCreate()override;
		void OnUpdate()override;
	};
}