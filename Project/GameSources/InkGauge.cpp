#include "stdafx.h"
#include "Project.h"
#include "Player.h"

namespace basecross
{
	void InkGauge::OnCreate()
	{
		auto& app = App::GetApp();
		auto stage = GetStage();
		auto path = app->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
		app->RegisterTexture(L"Gauge", path + L"Gauge.png"); // 画像ファイルを読み込んでアセットとして登録する

		//インク最大値をプレイヤーから取得する
		m_player = stage->GetSharedGameObject<Player>(L"player");
		if (m_player) m_maxInk = m_player->GetMaxInk();

		float ink = m_currentInk / m_maxInk;
		m_width = m_maxInk * 20 * ink;
		m_vertices =
		{
			{Vec3(m_offsetPos.x, m_offsetPos.y, 0), m_color,Vec2(0, 0)},
			{Vec3(m_offsetPos.x, m_height + m_offsetPos.y, 0) , m_color,Vec2(0, 1)},
			{Vec3(m_width + m_offsetPos.x, m_offsetPos.y, 0) , m_color,Vec2(1, 0)},
			{Vec3(m_width + m_offsetPos.x, m_height + m_offsetPos.y, 0) , m_color,Vec2(1, 1)}
		};
		m_indices = {
			0,1,2,
			1,3,2
		};
		m_draw = AddComponent<PCTSpriteDraw>(m_vertices, m_indices);
		m_draw->SetTextureResource(L"Gauge");
		m_trans = GetComponent<Transform>();

		SoundManager::Get().PlaySE(L"GAUGE", 0.50f);
	}

	void InkGauge::OnUpdate()
	{
		switch (m_state)
		{
		case A:
			GauseIncrease();
			break;
		case Update:
			GaugeUpdate();
			break;
		}
	}

	void InkGauge::GauseIncrease()
	{
		auto& app = App::GetApp();
		auto delta = app->GetElapsedTime();
		auto stage = GetStage();
		auto device = App::GetApp()->GetInputDevice();
		auto& pad = device.GetControlerVec()[0];

		m_player = stage->GetSharedGameObject<Player>(L"player");
		if (m_player)
		{
			m_maxInk = m_player->GetMaxInk();
			m_gaugeInk = m_maxInk / m_width;
			m_unitWidth = m_ink / m_gaugeInk * 2;

			m_vertices[2].position.x = m_unitWidth + m_offsetPos.x;
			m_vertices[3].position.x = m_unitWidth + m_offsetPos.x;

		}

		if (m_ink < m_maxInk)
		{
			m_ink += (m_maxInk / 3.0f) * delta;
		}
		else if (m_ink >= m_maxInk)
		{
			m_state = GauseState::Update;
		}

		if (pad.wPressedButtons & XINPUT_GAMEPAD_A)
		{
			m_ink = m_maxInk;
			m_state = GauseState::Update;
		}

		if (m_draw)
		{
			m_draw->UpdateVertices(m_vertices);
		}
	}

	void InkGauge::GaugeUpdate()
	{
		auto& app = App::GetApp();
		auto delta = app->GetElapsedTime();
		auto stage = GetStage();

		m_player = stage->GetSharedGameObject<Player>(L"player");
		if (m_player)
		{
			m_ink = m_player->GetInk();
			m_gaugeInk = m_maxInk / m_width;
			m_unitWidth = m_ink / m_gaugeInk * 2;

			m_vertices[2].position.x = m_unitWidth + m_offsetPos.x;
			m_vertices[3].position.x = m_unitWidth + m_offsetPos.x;

		}

		if (m_draw)
		{
			m_draw->UpdateVertices(m_vertices);
		}

		if (m_ink <= m_maxInk * 0.3f)
			ChangeGaugeColor();
	}

	void InkGauge::ChangeGaugeColor()
	{
		Col4 col = Col4(1.0f, 0.0f, 0.0f, 0.0f);
		m_draw->SetDiffuse(col);
		m_draw->SetEmissive(col);
	}


	//ゲージ後ろ
	void GaugeBack::OnCreate()
	{
		auto& app = App::GetApp();
		auto path = app->GetDataDirWString() + L"Texture\\"; // テクスチャのパスを構築
		app->RegisterTexture(L"Gauge", path + L"Gauge.png"); // 画像ファイルを読み込んでアセットとして登録する
		auto stage = GetStage();

		//インク最大値をプレイヤーから取得する
		m_player = stage->GetSharedGameObject<Player>(L"player");
		if (m_player) m_maxInk = m_player->GetMaxInk();

		float ink = m_currentInk / m_maxInk;

		m_width = m_maxInk * 20 * 2 * ink;
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
		m_draw = AddComponent<PCTSpriteDraw>(m_vertices, m_indices);

		m_draw->SetTextureResource(L"Gauge");
		//m_draw->SetDiffuse(m_color);
		m_trans = GetComponent<Transform>();
	}
}