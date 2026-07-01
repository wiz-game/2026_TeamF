/*!
@file Foo.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "GameProgressManager.h"
namespace basecross{

	void GameProgressManager::Initialize(UINT gameStageCount) {
		m_CurrentStage = -1;
		m_GameDates.clear();
		if (gameStageCount <= 0) return;
		for (UINT i = 0; i < gameStageCount; i++) {
			m_GameDates.push_back(GameProgressContext());
		}

		//最初のステージを解放
		m_GameDates[0].m_IsUnlocked = true;
	}

	bool GameProgressManager::IsUnlocked(UINT index)const {
		if (GetStageSize() <= index) return false;
		
		return m_GameDates[index].m_IsUnlocked;
	}
	void GameProgressManager::ClearCurrentStage() {
		if (m_GameDates.size() >= m_CurrentStage || m_CurrentStage == -1) return;
		m_GameDates[m_CurrentStage].m_IsClear = true;
	}
	bool GameProgressManager::IsClear(int stage)const {
		if (m_GameDates.size() >= stage || m_CurrentStage == -1) return false;
		return m_GameDates[stage].m_IsClear;
	}
}
//end basecross
