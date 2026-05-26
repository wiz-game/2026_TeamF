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

		Col4 m_color = Col4(0, 0, 0, 1);

	public:
		SpriteGeneric(const std::shared_ptr<Stage> stage) :
			GameObject(stage),
			m_width(0.0f),
			m_height(0.0f)
		{
		}

		virtual ~SpriteGeneric()
		{
		}

		void OnCreate()override;
		void OnUpdate()override;

		void SetSize(float width, float height)
		{
			m_width = width;
			m_height = height;
		}

		void SetColor(Col4 col)
		{
			m_color = col;
		}
	};
}