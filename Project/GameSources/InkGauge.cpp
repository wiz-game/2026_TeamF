#include "stdafx.h"
#include "Project.h"
#include "Player.h"

namespace basecross
{
	void InkGauge::OnCreate()
	{
		float ink = m_currentInk / m_maxInk;

		m_width = m_maxInk * 10 * ink;
		m_height = 25.0f / 2.0f;
		m_vertices =
		{
			{Vec3(m_offsetPos.x, m_offsetPos.y, 0), m_color,Vec2(0, 0)},
			{Vec3(m_offsetPos.x, m_height + m_offsetPos.y, 0) , m_color,Vec2(0, 1)},
			{Vec3(m_width + m_offsetPos.x, m_offsetPos.y, 0) , m_color,Vec2(1, 0)},
			{Vec3(m_width + m_offsetPos.x, m_height + m_offsetPos.y, 0) , m_color,Vec2(1, 1)}
		};
		m_indices = {
			0,1,2,
			2,1,3 
		};
		m_draw = AddComponent<PCTSpriteDraw>(m_vertices,m_indices);
		m_trans = GetComponent<Transform>();
	}

	void InkGauge::OnUpdate()
	{
		auto& app = App::GetApp();
		auto delta = app->GetElapsedTime();
		auto stage = GetStage();

		m_player = stage->GetSharedGameObject<Player>(L"player");
		if (m_player)
		{
			float actualInk = m_player->GetInk();
			float inkDec = 0.9f * delta;
			m_ink = m_maxInk / m_width;

			float ratio = (m_maxInk > 0) ? static_cast<float>(m_currentInk) / m_maxInk : 0.0f;
			//float targetWidth = 

			float interPol = 0.2f;

			m_vertices[2].position.x = (actualInk / m_ink) * 2 + m_offsetPos.x;
			m_vertices[3].position.x = (actualInk / m_ink) * 2 + m_offsetPos.x;

		}

		if (m_draw)
		{
			m_draw->UpdateVertices(m_vertices);
		}
	}
}