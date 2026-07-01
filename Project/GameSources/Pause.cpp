#pragma once
#include "stdafx.h"
#include "Project.h"
#include "Pause.h"

namespace basecross
{
	void Pause::OnCreate()
	{
		
	}

	void Pause::OnUpdate()
	{

	}

	void Pause::OnDraw()
	{
		if (IsPause())
		{
			m_pauseMenu->OnDraw();
		}
	
	}

	void Pause::PauseBase()
	{
		auto& app = App::GetApp();
		auto device = App::GetApp()->GetInputDevice();
		auto& pad = device.GetControlerVec()[0];

		if (pad.wPressedButtons & XINPUT_GAMEPAD_START)
		{
			//m_pauseMenu->SetPause(!pause);
			OnPause(!m_isPause);

			//BGMを中断
			SoundManager::Get().PauseBGM(!m_isPause);
		}

		if (IsPause())
		{
			m_pauseMenu->OnUpdate();
		}
	}

	void Pause::OnPause(bool isPause)
	{
		auto stage = GetStage();
		//bool pause = m_pauseMenu->GetPause();
		m_isPause = isPause;

		auto objs = stage->GetGameObjectVec();
		auto view = stage->GetView();
		auto camera = view->GetTargetCamera();
		for (auto& obj : objs)
		{
			obj->SetUpdateActive(!m_isPause);
		}
		//EffectManager::g_Instance->OnDraw();
	}

	bool Pause::IsPause() const
	{
		return m_isPause;
	}

	void Pause::ToPause()
	{
		
	}

	void Pause::ToOption()
	{
		m_pauseMenu->SetPause(false);
		m_optionMenu->SetOption(true);
	}

}