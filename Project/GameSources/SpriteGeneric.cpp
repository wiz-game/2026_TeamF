#include"stdafx.h"
#include"SpriteGeneric.h"

namespace basecross
{
	void SpriteGeneric::OnCreate()
	{
		auto& app = App::GetApp();
		auto stage = GetStage();
		auto path = app->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
		app->RegisterTexture(L"Gauge", path + L"Gauge.png"); // 画像ファイルを読み込んでアセットとして登録する

		float left = m_width;
		float right = left + m_width;
		float top = m_height;
		float bottom = top - m_height;

		m_vertices =
		{
			{Vec3(left, top, 0), m_color,Vec2(0, 0)},
			{Vec3(right, top, 0) , m_color,Vec2(1, 0)},
			{Vec3(left, bottom, 0) , m_color,Vec2(0, 1)},
			{Vec3(right, bottom, 0) , m_color,Vec2(1, 1)}
		};
		m_indices = {
			0,1,2,
			2,1,3
		};
		m_draw = AddComponent<PCTSpriteDraw>(m_vertices, m_indices);
		m_draw->SetTextureResource(L"Gauge");
		SetAlphaActive(true);

		m_trans = GetComponent<Transform>();
	}

	void SpriteGeneric::OnUpdate()
	{

	}
}