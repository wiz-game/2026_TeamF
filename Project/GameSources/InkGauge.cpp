#include "stdafx.h"
#include "Project.h"
#include "Player.h"

namespace basecross
{
	void InkGauge::OnCreate()
	{
		float ink = m_currentInk / m_maxInk;

		Vec2 offsetPos = Vec2(-100, -15);

		float width = 200 * ink;
		float height = 25.0f / 2.0f;
		m_vertices =
		{
			{Vec3(offsetPos.x, offsetPos.y, 0), m_color,Vec2(0, 0)},
			{Vec3(offsetPos.x, height + offsetPos.y, 0) , m_color,Vec2(0, 1)},
			{Vec3(width + offsetPos.x, offsetPos.y, 0) , m_color,Vec2(1, 0)},
			{Vec3(width + offsetPos.x, height + offsetPos.y, 0) , m_color,Vec2(1, 1)}
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
		float ink = m_currentInk / m_maxInk;
	}
}