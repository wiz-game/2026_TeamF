#pragma once
#include "stdafx.h"
#include "Player.h"

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
		float m_gaugeInk;
		float m_ink;

		float m_width;
		float m_height;
		float m_unitWidth;	//インク1あたりの横幅

		Vec2 m_offsetPos;

		enum GauseState
		{
			A,	//演出中
			Update	//通常
		};
		GauseState m_state = GauseState::A;

	public:
		InkGauge(const std::shared_ptr<Stage>stage) :
			GameObject(stage),
			m_currentInk(10.0f),
			m_maxInk(0.0f),
			m_gaugeInk(0.0f),
			m_ink(0.0f),
			m_width(0.0f),
			m_height(40.0f),
			m_unitWidth(0.0f),
			m_offsetPos(Vec2(-600.0f,325.0f))
		{
		}

		virtual ~InkGauge()
		{
		}

		void OnCreate()override;
		void OnUpdate()override;

		void GauseIncrease();

		void GaugeUpdate();
		void ChangeGaugeColor();
	};

	//ゲージ後ろ
	class GaugeBack :public GameObject
	{
		std::shared_ptr<PCTSpriteDraw> m_draw;
		std::shared_ptr<Transform> m_trans;

		std::shared_ptr<Player> m_player;

		std::vector<VertexPositionColorTexture> m_vertices;
		std::vector<uint16_t> m_indices;

		Col4 m_color = Col4(1, 1, 1, 1);

		float m_currentInk;
		float m_maxInk;

		float m_width;
		float m_height;

		Vec2 m_offsetPos;
	public:
		GaugeBack(const std::shared_ptr<Stage>stage) :
			GameObject(stage),
			m_currentInk(10.0f),
			m_maxInk(0.0f),
			m_width(0.0f),
			m_height(40.0f),
			m_offsetPos(Vec2(-600.0f, 325.0f))
		{
		}

		~GaugeBack()
		{
		}

		void OnCreate()override;
	};
}