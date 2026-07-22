/*!
@file Foo.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "Singleton.h"
namespace basecross{
	struct GameProgressContext {
		bool m_IsClear = false;
		bool m_IsUnlocked = false;
	};
	class GameProgressManager : public SingletonBase<GameProgressManager> {
		friend class SingletonBase<GameProgressManager>;
		
		vector<GameProgressContext> m_GameDates;
		int m_CurrentStage;
	public:
		void Initialize(UINT gameStageCount);

		size_t GetStageSize() const{ return m_GameDates.size(); }
		bool IsUnlocked(UINT index)const;

		void SetCurrentStage(int stage) { m_CurrentStage = stage; }
		int GetCurrentStage()const { return m_CurrentStage; }
		bool IsExistsNextStage()const { return ( m_GameDates.size() - 1 ) > m_CurrentStage; }

		void ClearCurrentStage();
		bool IsClear(int stage)const;
	};
}
//end basecross
