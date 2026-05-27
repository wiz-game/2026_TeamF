#pragma once
#include "stdafx.h"

namespace basecross
{
	class SpriteGeneric :public GameObject
	{
		std::shared_ptr<PCTSpriteDraw>m_draw;
		std::shared_ptr<Transform>m_trans;

		std::vector<VertexPositionColorTexture> m_vertices;
		std::vector<uint16_t> m_indices;

		float m_width;
		float m_height;
		Vec2 m_pivot = Vec2(0.5f, 0.5f);

		Vec3 m_pos = Vec3(0, 0, 0);

		Col4 m_color = Col4(0, 0, 0, 1);

	public:
		SpriteGeneric(const std::shared_ptr<Stage> stage) :
			GameObject(stage),
			m_width(100.0f),
			m_height(100.0f)
		{
		}

		virtual ~SpriteGeneric()
		{
		}

		void OnCreate()override;

		void SetTexture(const std::wstring tex)
		{
			if (m_draw)
			{
				m_draw->SetTextureResource(tex);
			}
		}

		void SetSize(float width, float height, const Vec2& pivot)
		{
			m_width = width;
			m_height = height;
			m_pivot = pivot;

			float left = m_width;
			float right = left + m_width;
			float top = m_height;
			float bottom = top - m_height;

			m_vertices[0].position = Vec3(-m_pivot.x * m_width, m_pivot.y * m_height, 0.0f);
			m_vertices[1].position = Vec3((-m_pivot.x + 1.0f) * m_width, m_pivot.y * m_height, 0.0f);
			m_vertices[2].position = Vec3(-m_pivot.x * m_width, -(1.0f - m_pivot.y) * m_height, 0.0f);
			m_vertices[3].position = Vec3((-m_pivot.x + 1.0f) * m_width, -(1.0f - m_pivot.y) * m_height, 0.0f);

			//};
			if (m_draw)
			{
				m_draw->UpdateVertices(m_vertices);
			}
		}

		void SetPos(Vec3 pos)
		{
			m_pos = pos;
			m_trans->SetPosition(m_pos);
		}

		void SetColor(Col4 col)
		{
			m_color = col;
		}
	};
}