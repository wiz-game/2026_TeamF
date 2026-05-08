/*!
@file Foo.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "GameProgressManager.h"
namespace basecross{

	void GameProgressManager::Initialize(UINT gameStageCount) {
		m_GameDates.clear();
		if (gameStageCount <= 0) return;
		for (int i = 0; i < gameStageCount; i++) {
			m_GameDates.push_back(GameProgressContext());
		}

		//最初のステージを解放
		m_GameDates[0].m_IsUnlocked = true;
	}

	bool GameProgressManager::IsUnlocked(UINT index)const {
		if (GetStageSize() <= index) return false;
		
		return m_GameDates[index].m_IsUnlocked;
	}
}
//end basecross
